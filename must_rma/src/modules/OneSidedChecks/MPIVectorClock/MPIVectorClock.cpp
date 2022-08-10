/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MPIVectorClock.cpp
 *       @see must::MPIVectorClock.
 *
 *  @date 22.05.2017
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"
#include "MPIVectorClock.h"
#include "MustEnums.h"
#include "MustDefines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mustFeaturetested.h"

#include <sstream>
#include <fstream>

using namespace must;

mGET_INSTANCE_FUNCTION(MPIVectorClock)
mFREE_INSTANCE_FUNCTION(MPIVectorClock)
mPNMPI_REGISTRATIONPOINT_FUNCTION(MPIVectorClock)

//=============================
// Constructor.
//=============================
MPIVectorClock::MPIVectorClock (const char* instanceName)
    : ModuleBase<MPIVectorClock, I_MPIVectorClock> (instanceName),
    myRank(-1),
    myClk()
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUBMODULES 8
    if (subModInstances.size() < NUM_SUBMODULES)
    {
            std::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
            assert (0);
    }
    if (subModInstances.size() > NUM_SUBMODULES)
    {
            for (std::vector<I_Module*>::size_type i = NUM_SUBMODULES; i < subModInstances.size(); i++)
                destroySubModuleInstance (subModInstances[i]);
    }

    myPIdMod = (I_ParallelIdAnalysis*) subModInstances[0];
    myLogger = (I_CreateMessage*) subModInstances[1];
    myConsts = (I_BaseConstants*) subModInstances[2];
    myArgMod = (I_ArgumentAnalysis*) subModInstances[3];
    myDatMod = (I_DatatypeTrack*) subModInstances[4];
    myReqMod = (I_RequestTrack*) subModInstances[5];
    myLIdMod = (I_LocationAnalysis*) subModInstances[6];
    myCommMod = (I_CommTrack*) subModInstances[7];
    myTSanMod = (I_TSan*) subModInstances[8];

    //Initialize module data
    getWrapperFunction("notifySync", (GTI_Fct_t*) &myNotifySyncFunc);
    assert(myNotifySyncFunc);
}

//=============================
// init
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::init (MustParallelId pId) {
    // get own rank
    myRank = myPIdMod->getInfoForId(pId).rank;

    // create process vector clock
    myNProcs = myCommMod->getComm(pId, myCommMod->getWorldHandle())->getGroup()->getSize();
    myClk.resize(myNProcs);
    std::fill(myClk.begin(), myClk.end(), 0);

    // initialize local and remote RMA call matrix
    myRMACallMatrix.resize(myNProcs * myNProcs);
    myRemoteRMACallMatrix.resize(myNProcs * myNProcs);
    std::fill(myRMACallMatrix.begin(), myRMACallMatrix.end(), 0);
    std::fill(myRemoteRMACallMatrix.begin(), myRemoteRMACallMatrix.end(), 0);

    myRMACallMatrixSize = myNProcs * myNProcs;

    // use a private world communicator for exchange of vector clocks
    //    XMPI_Comm_dup(MPI_COMM_WORLD, &myCommWorld);
    // temporary workaround, since the CommRewrite module is not working:
    PNMPI_modHandle_t handle;
    PNMPI_Service_descriptor_t service;
    PNMPI_Service_Fct_t fct;

    int err = PNMPI_Service_GetModuleByName("split_processes", &handle);
    assert(err == PNMPI_SUCCESS);
    err = PNMPI_Service_GetServiceByName(handle, "SplitMod_getMySetComm", "p", &service);
    assert(err == PNMPI_SUCCESS);
    MPI_Comm fakeComm;
    ((int(*)(void*)) service.fct) (&fakeComm);
    XMPI_Comm_dup(fakeComm, &myCommWorld);

    // get placement module used for waiting for RMA calls
    getPlaceMod(&myPlaceMod);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// tick
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::tick() {
    if (myRank == -1 || myClk.size() <= myRank)
        return GTI_ANALYSIS_FAILURE;

    myClk[myRank]++;

#ifdef MUST_DEBUG
    printMPIVectorClock();
#endif

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addPendingRMACall
//=============================
void MPIVectorClock::addPendingRMACall(int targetRank) {
    myRMACallMatrix[(myRank*myNProcs)+targetRank]++;
}

//=============================
// receiveRMACall
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::receiveRMACall() {
    // decrement the counter of waiting messages
    myNumWaitCalls--;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// get
//=============================
const Clock& MPIVectorClock::get() {
    return myClk;
}

//=============================
// getLocalEntry
//=============================
ClockValue MPIVectorClock::getLocalEntry() {
    if (myRank == -1 || myClk.size() <= myRank)
        return 0;
    else
        return myClk[myRank];
}

//=============================
// getTSanSyncClock
//=============================
void* MPIVectorClock::getTSanSyncClock(int clock) {
    // look up corresponding annotation for MPI rank and given clock value
    std::map<int, void*>::iterator it = mySignalAddrs.find(clock);
    if (it == mySignalAddrs.end()) {
        assert(0);
        return NULL;
    }

    return it->second;
}

//=============================
// storeTSanSyncClock
//=============================
void MPIVectorClock::storeTSanSyncClock(MustParallelId pId, MustLocationId lId, void* ann) {
    // store current VC
    //myTSanMod->annotateHappensBeforeUC(pId, lId, ann);

    mySignalAddrs.insert(std::make_pair(myClk[myRank], ann));
}

//=============================
// merge
//=============================
void MPIVectorClock::merge(const Clock& clock, RMACallMatrix& remoteRMACallMatrix, MustParallelId pId, MustLocationId lId, void* ann) {
    if (myClk.size() != clock.size())
        assert(0);

    int newRMACalls = 0;

    // get number of in-flight RMA calls
    for (int i = 0; i < myClk.size(); ++i) {
        unsigned int newValue = std::max(myRemoteRMACallMatrix[(i*myNProcs)+myRank],
                myRMACallMatrix[(i*myNProcs)+myRank]);
        newRMACalls += newValue - myRMACallMatrix[(i*myNProcs)+myRank];
        myRMACallMatrix[(i*myNProcs)+myRank] = newValue;
    }

    // add new RMA calls
    myNumWaitCalls += newRMACalls;

    // wait for all in-flight RMA calls here
    while (myNumWaitCalls > 0) {
        myPlaceMod->testIntralayer();
    }

    // perform max operation per entry
    Clock::iterator itLocal = myClk.begin();
    Clock::const_iterator itRemote = clock.begin();

    while (itLocal != myClk.end() && itRemote != clock.end()) {
        if (*itLocal < *itRemote) {
            // vector clock is updated at corresponding entry
            *itLocal = *itRemote;
            // determine remote rank
            int remoteRank = itLocal - myClk.begin();
            // notify listeners of synchronization with remote rank
            //myNotifySyncFunc(remoteRank, myRank);

            // TODO: garbage collection: delete last signals that are not needed anymore
            //       required: record which clock value of myRank all other ranks have and only
            //       clean up old values that are definitely not needed anymore
            // TODO: delete unused addresses (ref counting)
        }
        ++itLocal;
        ++itRemote;
    }

#if MUST_DEBUG
    std::cout << "merge (annotation addr: " << ann << "):";
    printMPIVectorClock();
#endif
}

//=============================
// send
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::send(
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        MustCommType comm,
        void* ann) {

    tick();
    storeTSanSyncClock(pId, lId, ann);

    // translate to world rank
    int realDest = translateRank(myCommMod->getComm(pId, comm), dest);

    XMPI_Send(&myClk[0], myClk.size(), MPI_UNSIGNED_LONG_LONG,
              realDest, 0, myCommWorld);
    XMPI_Send(&myRMACallMatrix[0], myRMACallMatrixSize, MPI_UNSIGNED,
              realDest, 0, myCommWorld);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recv
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::recv(
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustCommType comm,
        void* ann) {

    tick();

    // annotate happens-before arc between app thread and tool thread in both directions
    myTSanMod->annotateHappensAfter(pId, lId, ann);
    myTSanMod->annotateHappensBefore(pId, lId, ann);

    // translate to world rank
    int realSource = translateRank(myCommMod->getComm(pId, comm), source);

    Clock remoteClock(myClk.size());
    XMPI_Recv(&remoteClock[0], remoteClock.size(), MPI_UNSIGNED_LONG_LONG,
              realSource, 0, myCommWorld, MPI_STATUS_IGNORE);

    XMPI_Recv(&myRemoteRMACallMatrix[0], myRMACallMatrixSize, MPI_UNSIGNED,
              realSource, 0, myCommWorld, MPI_STATUS_IGNORE);

    merge(remoteClock, myRemoteRMACallMatrix, pId, lId, ann);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// sendRecv
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::sendRecv(
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        int source,
        MustCommType comm,
        void* ann) {

    tick();

    // translate to world rank
    int realDest = translateRank(myCommMod->getComm(pId, comm), dest);
    int realSource = translateRank(myCommMod->getComm(pId, comm), source);

    Clock remoteClock(myClk.size());
    XMPI_Sendrecv(&myClk[0], myClk.size(), MPI_UNSIGNED_LONG_LONG, realDest, 0,
                  &remoteClock[0], remoteClock.size(), MPI_UNSIGNED_LONG_LONG, realSource, 0,
                  myCommWorld, MPI_STATUS_IGNORE);

    XMPI_Sendrecv(&myRMACallMatrix[0],       myRMACallMatrixSize, MPI_UNSIGNED, realDest, 0,
                  &myRemoteRMACallMatrix[0], myRMACallMatrixSize, MPI_UNSIGNED, realSource, 0,
                  myCommWorld, MPI_STATUS_IGNORE);

    merge(remoteClock, myRemoteRMACallMatrix, pId, lId, ann);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// isend
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::isend(
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        MustCommType comm,
        MustRequestType request,
        void* ann) {

    tick();
    storeTSanSyncClock(pId, lId, ann);

    // translate to world rank
    int realDest = translateRank(myCommMod->getComm(pId, comm), dest);

    MPI_Request vcReq;
    XMPI_Isend(&myClk[0], myClk.size(), MPI_UNSIGNED_LONG_LONG,
               realDest, 0, myCommWorld, &vcReq);

    MPI_Request rmaReq;
    XMPI_Isend(&myRMACallMatrix[0], myRMACallMatrixSize, MPI_UNSIGNED,
               realDest, 0, myCommWorld, &rmaReq);

    // map request of original Isend to request of vector clock and
    // RMA call matrix Isend
    myIsendRequestMap[request] = std::make_pair(vcReq, rmaReq);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// completedIsend
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::completedIsend(
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType request) {

    tick();

    std::map<MustRequestType, std::pair<MPI_Request, MPI_Request>>::iterator it;
    it = myIsendRequestMap.find(request);

    if (it == myIsendRequestMap.end())
        return GTI_ANALYSIS_FAILURE;

    // wait for transmission of vector clock and RMA call matrix
    XMPI_Wait(&(it->second.first), MPI_STATUS_IGNORE);
    XMPI_Wait(&(it->second.second), MPI_STATUS_IGNORE);

    myIsendRequestMap.erase(it);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// irecv
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::irecv(
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustCommType comm,
        MustRequestType request) {

    tick();

    // just store the communicator for a later rank translation,
    // the actual receive is delayed until the corresponding wait
    // call occurs
    myIrecvRequestMap[request] = comm;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// completedIrecv
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::completedIrecv(
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustRequestType request,
        void* ann) {

    tick();

    std::map<MustRequestType, MustCommType>::iterator it;
    it = myIrecvRequestMap.find(request);

    if (it == myIrecvRequestMap.end())
        return GTI_ANALYSIS_FAILURE;

    // translate to world rank
    int realSource = translateRank(myCommMod->getComm(pId, it->second), source);

    Clock remoteClock(myClk.size());
    XMPI_Recv(&remoteClock[0], remoteClock.size(), MPI_UNSIGNED_LONG_LONG,
              realSource, 0, myCommWorld, MPI_STATUS_IGNORE);

    XMPI_Recv(&myRemoteRMACallMatrix[0], myRMACallMatrixSize, MPI_UNSIGNED,
              realSource, 0, myCommWorld, MPI_STATUS_IGNORE);
    merge(remoteClock, myRemoteRMACallMatrix, pId, lId, ann);

    myIrecvRequestMap.erase(it);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// coll
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::coll(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustCommType comm,
                    int hasRequest,
                    MustRequestType request,
                    void* ann
                    ) {
    tick();
    storeTSanSyncClock(pId, lId, ann);

    if (hasRequest) {
        // We delay the MPIVectorClock exchange until the corresponding request is completed.
        myIcollRequestMap[request] = std::make_pair(comm, myClk);
    } else {
        Clock reducedClock(myClk.size());
        clockReduction(comm, pId, lId, myClk, myRemoteRMACallMatrix, reducedClock, ann);

        // The reduced clock gets the new clock.
        merge(reducedClock, myRemoteRMACallMatrix, pId, lId, ann);
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// completedIcoll
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::completedIcoll(
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType request,
        void* ann) {

    std::map<MustRequestType, std::pair<MustCommType, Clock>>::iterator it;
    it = myIcollRequestMap.find(request);

    if (it == myIcollRequestMap.end())
        return GTI_ANALYSIS_FAILURE;

    tick();
    storeTSanSyncClock(pId, lId, ann);

    MustCommType comm = it->second.first;
    Clock reducedClock(myClk.size());
    clockReduction(comm, pId, lId, it->second.second, myRemoteRMACallMatrix, reducedClock, ann);

    merge(reducedClock, myRemoteRMACallMatrix, pId, lId, ann);
    

    myIcollRequestMap.erase(it);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// clockReduction
//=============================
void MPIVectorClock::clockReduction(MustCommType comm,
        MustParallelId pId,
        MustLocationId lId,
        const Clock& sendClock,
        RMACallMatrix& remoteRMACallMatrix,
        Clock& reducedClock,
        void* ann) {

    MPI_Request reqs[2];

    // Perform vector clock reduction (max reduction on all vector clock entries)
    XMPI_Iallreduce(&sendClock[0], &reducedClock[0], sendClock.size(), MPI_UNSIGNED_LONG_LONG,
            MPI_MAX, MUST_Comm_i2m(comm), &reqs[0]);

    // Get ranks (translated to world rank) associated with this communicator
//    const std::vector<int>& groupMapping = myCommMod->getComm(pId, comm)->getGroup()->getMapping();

    // Perform RMA call matrix reduction (max reduction on all matrix entries)
    XMPI_Iallreduce(&myRMACallMatrix[0], &remoteRMACallMatrix[0], myRMACallMatrixSize, MPI_UNSIGNED,
            MPI_MAX, MUST_Comm_i2m(comm), &reqs[1]);

    // Make progress in tool thread to receive outstanding RMA calls
    // If a remote tool thread is sending a lot of data, then it might block until the data is
    // arrived at the target tool thread. Therefore, the target tool thread continuously has to check
    // for incoming messages instead of stalling in the allreduce calls.
    int flag = 0;
    while (flag == 0) {
        XMPI_Testall(2, reqs, &flag, MPI_STATUSES_IGNORE);
        myPlaceMod->testIntralayer();
    }
}

//=============================
// translateRank
//=============================
int MPIVectorClock::translateRank (I_Comm* comm, int rank)
{
    int ret;
    if (rank != myConsts->getAnySource()) {
        if (!comm->isIntercomm())
            comm->getGroup()->translate(rank, &ret);
        else
            comm->getRemoteGroup()->translate(rank, &ret);
    } else {
        ret = rank;
    }

    return ret;
}

//=============================
// printMPIVectorClock
//=============================
void MPIVectorClock::printMPIVectorClock() {
    std::stringstream stream;
    for (Clock::iterator it = myClk.begin(); it != myClk.end(); ++it) {
        if (it - myClk.begin() == myRank)
            stream << *it << "*,";
        else
            stream << *it << ",";
    }

    std::cout << "[" << stream.str().substr(0, stream.str().size() - 1) << "]" << std::endl;
}

//=============================
// happensBeforeSync
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::happensBeforeSync(void* ann) {
    // annotate happens-before arc from application thread to tool thread
    myTSanMod->annotateHappensAfter(0, 0, ann);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// happensAfterSync
//=============================
GTI_ANALYSIS_RETURN MPIVectorClock::happensAfterSync(void* ann) {
    // annotate happens-before arc from tool thread to application thread
    myTSanMod->annotateHappensBefore(0, 0, ann);

    return GTI_ANALYSIS_SUCCESS;
}


//=============================
// Destructor.
//=============================
MPIVectorClock::~MPIVectorClock (void)
{
    if (myPIdMod)
        destroySubModuleInstance ((I_Module*) myPIdMod);
    myPIdMod = NULL;

    if (myLogger)
        destroySubModuleInstance ((I_Module*) myLogger);
    myLogger = NULL;

    if (myArgMod)
        destroySubModuleInstance ((I_Module*) myArgMod);
    myArgMod = NULL;

    if (myDatMod)
        destroySubModuleInstance ((I_Module*) myDatMod);
    myDatMod = NULL;

    if (myReqMod)
        destroySubModuleInstance ((I_Module*) myReqMod);
    myReqMod = NULL;

    if (myCommMod)
        destroySubModuleInstance ((I_Module*) myCommMod);
    myCommMod = NULL;

    if (myTSanMod)
        destroySubModuleInstance ((I_Module*) myTSanMod);
    myTSanMod = NULL;
}
