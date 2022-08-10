/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file AppThrAnn.cpp
 *       @see must::AppThrAnn.
 *
 *  @date 27.06.2017
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"
#include "AppThrAnn.h"
#include "MustEnums.h"
#include "MustDefines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sstream>
#include <fstream>

using namespace must;

mGET_INSTANCE_FUNCTION(AppThrAnn)
mFREE_INSTANCE_FUNCTION(AppThrAnn)
mPNMPI_REGISTRATIONPOINT_FUNCTION(AppThrAnn)

//=============================
// Constructor.
//=============================
AppThrAnn::AppThrAnn (const char* instanceName)
    : ModuleBase<AppThrAnn, I_AppThrAnn> (instanceName),
    myPropagateGetCall(0),
    myPropagatePutCall(0),
    myPropagateAccumulateCall(0),
    myPropagateGetAccumulateCall(0),
    myPropagateWinUnlock(0),
    myPropagateWinUnlockAll(0),
    myPropagateWinFence(0),
    myPropagateWinStart(0),
    myPropagateWinComplete(0),
    myPropagateWinPost(0),
    myPropagateWinWait(0),
    myPropagateWinFlush(0),
    myPropagateWinFlushLocal(0),
    myPropagateWinFlushAll(0),
    myPropagateWinFlushLocalAll(0),
    myPropagateSignal(0)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUBMODULES 4
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
    myConsts = (I_BaseConstants*) subModInstances[1];
    myWinMod = (I_WinTrack*) subModInstances[2];
    myTSanMod = (I_TSan*) subModInstances[3];

    //Initialize module data
    //Nothing to do
    getWrapperFunction("propagateGetCall",(GTI_Fct_t*) &myPropagateGetCall);
    assert(myPropagateGetCall);
    getWrapperFunction("propagatePutCall",(GTI_Fct_t*) &myPropagatePutCall);
    assert(myPropagatePutCall);
    getWrapperFunction("propagateAccumulateCall",(GTI_Fct_t*) &myPropagateAccumulateCall);
    assert(myPropagateAccumulateCall);
    getWrapperFunction("propagateGetAccumulateCall",(GTI_Fct_t*) &myPropagateGetAccumulateCall);
    assert(myPropagateGetAccumulateCall);
    getWrapperFunction("propagateGetCall",(GTI_Fct_t*) &myPropagateGetCall);
    assert(myPropagateGetCall);
    getWrapperFunction("propagatePutCall",(GTI_Fct_t*) &myPropagatePutCall);
    assert(myPropagatePutCall);
    getWrapperFunction("propagateAccumulateCall",(GTI_Fct_t*) &myPropagateAccumulateCall);
    assert(myPropagateAccumulateCall);
    getWrapperFunction("propagateGetAccumulateCall",(GTI_Fct_t*) &myPropagateGetAccumulateCall);
    assert(myPropagateGetAccumulateCall);
    getWrapperFunction("propagateWinCreate",(GTI_Fct_t*) &myPropagateWinCreate);
    assert(myPropagateWinCreate);
    getWrapperFunction("propagateWinLock",(GTI_Fct_t*) &myPropagateWinLock);
    assert(myPropagateWinLock);
    getWrapperFunction("propagateWinSync",(GTI_Fct_t*) &myPropagateWinSync);
    assert(myPropagateWinSync);
    getWrapperFunction("propagateWinUnlock",(GTI_Fct_t*) &myPropagateWinUnlock);
    assert(myPropagateWinUnlock);
    getWrapperFunction("propagateWinUnlockAll",(GTI_Fct_t*) &myPropagateWinUnlockAll);
    assert(myPropagateWinUnlockAll);
    getWrapperFunction("propagateWinFence",(GTI_Fct_t*) &myPropagateWinFence);
    assert(myPropagateWinFence);
    getWrapperFunction("propagateWinStart",(GTI_Fct_t*) &myPropagateWinStart);
    assert(myPropagateWinStart);
    getWrapperFunction("propagateWinComplete",(GTI_Fct_t*) &myPropagateWinComplete);
    assert(myPropagateWinComplete);
    getWrapperFunction("propagateWinPost",(GTI_Fct_t*) &myPropagateWinPost);
    assert(myPropagateWinPost);
    getWrapperFunction("propagateWinWait",(GTI_Fct_t*) &myPropagateWinWait);
    assert(myPropagateWinWait);
    getWrapperFunction("propagateWinFlush",(GTI_Fct_t*) &myPropagateWinFlush);
    assert(myPropagateWinFlush);
    getWrapperFunction("propagateWinFlushLocal",(GTI_Fct_t*) &myPropagateWinFlushLocal);
    assert(myPropagateWinFlushLocal);
    getWrapperFunction("propagateWinFlushAll",(GTI_Fct_t*) &myPropagateWinFlushAll);
    assert(myPropagateWinFlushAll);
    getWrapperFunction("propagateWinFlushLocalAll",(GTI_Fct_t*) &myPropagateWinFlushLocalAll);
    assert(myPropagateWinFlushLocalAll);

    getWrapperFunction("propagateSend",(GTI_Fct_t*) &myPropagateSend);
    assert(myPropagateSend);
    getWrapperFunction("propagateRecv",(GTI_Fct_t*) &myPropagateRecv);
    assert(myPropagateRecv);
    getWrapperFunction("propagateSendRecv",(GTI_Fct_t*) &myPropagateSendRecv);
    assert(myPropagateSendRecv);
    getWrapperFunction("propagateIsend",(GTI_Fct_t*) &myPropagateIsend);
    assert(myPropagateIsend);
    getWrapperFunction("propagateIrecv",(GTI_Fct_t*) &myPropagateIrecv);
    assert(myPropagateIrecv);
    getWrapperFunction("propagateColl",(GTI_Fct_t*) &myPropagateColl);
    assert(myPropagateColl);
    getWrapperFunction("propagateCompletedIsend",(GTI_Fct_t*) &myPropagateCompletedIsend);
    assert(myPropagateCompletedIsend);
    getWrapperFunction("propagateCompletedIrecv",(GTI_Fct_t*) &myPropagateCompletedIrecv);
    assert(myPropagateCompletedIrecv);
    getWrapperFunction("propagateCompletedIcoll",(GTI_Fct_t*) &myPropagateCompletedIcoll);
    assert(myPropagateCompletedIcoll);

    getWrapperFunction("happensBeforeSync",(GTI_Fct_t*) &myHappensBeforeSync);
    assert(myHappensBeforeSync);
    getWrapperFunction("happensAfterSync",(GTI_Fct_t*) &myHappensAfterSync);
    assert(myHappensAfterSync);

    getWrapperFunction("propagateSignal",(GTI_Fct_t*) &myPropagateSignal);
    assert(myPropagateSignal);

    // Create data pool for annotation addresses
    myAnnPool = new DataPool<AnnData, 4>();
}


//=============================
// addGetOperation
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::addGetOperation (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType originaddr,
                int origincount,
                MustDatatypeType origintype,
                int target,
                int targetdisp,
                int targetcount,
                MustDatatypeType targettype,
                MustWinType win,
                MustRequestType requesttype
                ) {

    void *functionaddr;
    PNMPI_Service_GetFunctionAddress(&functionaddr);

    happensBeforeSync();
    myPropagateGetCall(pId, lId, originaddr, origincount, origintype, target, targetdisp,
            targetcount, targettype, win, requesttype, genTLCAddr(), functionaddr);
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addPutOperation
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::addPutOperation (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType originaddr,
                int origincount,
                MustDatatypeType origintype,
                int target,
                int targetdisp,
                int targetcount,
                MustDatatypeType targettype,
                MustWinType win,
                MustRequestType requesttype
                ) {

    void *functionaddr;
    PNMPI_Service_GetFunctionAddress(&functionaddr);

    happensBeforeSync();
    myPropagatePutCall(pId, lId, originaddr, origincount, origintype, target, targetdisp,
            targetcount, targettype, win, requesttype, genTLCAddr(), functionaddr);
    happensAfterSync();


    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addAccumulateOperation
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::addAccumulateOperation (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType originaddr,
                int origincount,
                MustDatatypeType origintype,
                int target,
                int targetdisp,
                int targetcount,
                MustDatatypeType targettype,
                MustOpType op,
                MustWinType win,
                MustRequestType requesttype
                ) {

    void *functionaddr;
    PNMPI_Service_GetFunctionAddress(&functionaddr);

    happensBeforeSync();
    myPropagateAccumulateCall(pId, lId, originaddr, origincount, origintype, target, targetdisp,
            targetcount, targettype, op, win, requesttype, genTLCAddr(), functionaddr);
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addGetAccumulateOperation
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::addGetAccumulateOperation (
        MustParallelId pId,
                MustLocationId lId,
                MustAddressType originaddr,
                int origincount,
                MustDatatypeType origintype,
                MustAddressType resultaddr,
                int resultcount,
                MustDatatypeType resulttype,
                int target,
                int targetdisp,
                int targetcount,
                MustDatatypeType targettype,
                MustOpType op,
                MustWinType win,
                MustRequestType requesttype
                ) {

    void *functionaddr;
    PNMPI_Service_GetFunctionAddress(&functionaddr);
    
    happensBeforeSync();
    myPropagateGetAccumulateCall(pId, lId, originaddr, origincount, origintype, resultaddr, resultcount,
            resulttype, target, targetdisp, targetcount, targettype, op, win, requesttype, genTLCAddr(), functionaddr);
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winLock
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winLock (
        MustParallelId pId,
        MustLocationId lId,
        int lock_type,
        int rank,
        MustWinType win) {
    int realRank = translateRank(myWinMod->getWin(pId, win)->getComm(), rank);

    AnnData* lock = NULL;
    // if this is a local win lock, acquire RW lock 
    if (lock_type == MPI_LOCK_EXCLUSIVE && realRank == myPIdMod->getInfoForId(pId).rank) {
        std::map<MustWinType, AnnData*>::iterator it;
        it = myRWLockMap.find(win);
        if (it != myRWLockMap.end()) {
            lock = it->second;
        } else {
            lock = new AnnData();
            myRWLockMap.insert(std::make_pair(win, lock));
        }

        myTSanMod->annotateRWLockAcquired(pId, lId, lock, true);
    }

    happensBeforeSync();
    myPropagateWinLock(pId, lId, lock_type, rank, win, lock);
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winSync
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winSync (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinSync(pId, lId, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winUnlock
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winUnlock (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win) {

    AnnData* lock = NULL;
    // if this is a local win lock, release RW lock 
    if (myRWLockMap.find(win) != myRWLockMap.end() && rank == myPIdMod->getInfoForId(pId).rank) {
        lock = myRWLockMap[win];
        myTSanMod->annotateRWLockReleased(pId, lId, lock, true);
    }
    
    happensBeforeSync();
    myPropagateWinUnlock(pId, lId, rank, win, lock);
    happensAfterSync();
    
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winUnlockAll
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winUnlockAll (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinUnlockAll(pId, lId, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winCreate
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winCreate (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinCreate(pId, lId, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winFence
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winFence (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinFence(pId, lId, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winStart
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winStart (
        MustParallelId pId,
        MustLocationId lId,
        MustGroupType group,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinStart(pId, lId, group, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}


//=============================
// winComplete
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winComplete (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinComplete(pId, lId, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winPost
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winPost (
        MustParallelId pId,
        MustLocationId lId,
        MustGroupType group,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinPost(pId, lId, group, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}


//=============================
// winWait
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winWait (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinWait(pId, lId, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winFlush
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winFlush (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinFlush(pId, lId, rank, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winFlushLocal
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winFlushLocal (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinFlushLocal(pId, lId, rank, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winFlushAll
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winFlushAll (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinFlushAll(pId, lId, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winFlushLocalAll
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::winFlushLocalAll (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win) {

    happensBeforeSync();
    myPropagateWinFlushLocalAll(pId, lId, win, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// send
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::send(
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        MustCommType comm) {

    myPropagateSend(pId, lId, dest, comm, genTLCAddr());
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recv
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::recv(
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustCommType comm) {

    happensBeforeSync();
    myPropagateRecv(pId, lId, source, comm, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// sendRecv
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::sendRecv(
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        int source,
        MustCommType comm) {

    happensBeforeSync();
    myPropagateSendRecv(pId, lId, dest, source, comm, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// isend
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::isend(
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        MustCommType comm,
        MustRequestType request) {

    // store send as pending, the completion is signalled
    // later by myPropagateCompletedIsend
    myPendingSends.insert(request);
    myPropagateIsend(pId, lId, dest, comm, request, genTLCAddr());

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// irecv
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::irecv(
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustCommType comm,
        MustRequestType request) {

    // store recv as pending, the completion is signalled
    // later by myPropagateCompletedIrecv
    myPendingRecvs.insert(request);
    myPropagateIrecv(pId, lId, source, comm, request);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// coll
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::coll(
                   MustParallelId pId,
                   MustLocationId lId,
                   MustCommType comm,
                   int hasRequest,
                   MustRequestType request
                   ) {

    if (hasRequest)
        myPendingColls.insert(request);

    happensBeforeSync();
    myPropagateColl(pId, lId, comm, hasRequest, request, genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// completedRequest
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::completedRequest (
		MustParallelId pId,
		MustLocationId lId,
                int source,
		MustRequestType request)
{
    if (myPendingSends.find(request) != myPendingSends.end()) {
        myPendingSends.erase(request);

        myPropagateCompletedIsend(pId, lId, request);
    } else if (myPendingRecvs.find(request) != myPendingRecvs.end()) {
        myPendingRecvs.erase(request);

        happensBeforeSync();
        myPropagateCompletedIrecv(pId, lId, source, request, genTLCAddr());
        happensAfterSync();
    } else if (myPendingColls.find(request) != myPendingColls.end()) {
        myPendingColls.erase(request);

        happensBeforeSync();
        myPropagateCompletedIcoll(pId, lId, request, genTLCAddr());
        happensAfterSync();
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// completedRequests
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::completedRequests (
		MustParallelId pId,
		MustLocationId lId,
                int* sources,
		MustRequestType *requests,
		int count)
{
    for (int i = 0; i < count; i++)
    {
        GTI_ANALYSIS_RETURN ret = completedRequest(pId, lId, sources[i], requests[i]);
        if (ret != GTI_ANALYSIS_SUCCESS)
            return ret;
    }

    return GTI_ANALYSIS_SUCCESS;
}


//=============================
// annotateSignal
//=============================
GTI_ANALYSIS_RETURN AppThrAnn::annotateSignal() {
    happensBeforeSync();
    myPropagateSignal(genTLCAddr());
    happensAfterSync();

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// happensBeforeSync
//=============================
void AppThrAnn::happensBeforeSync() {
    AnnData* annData = new AnnData();
    myTSanMod->annotateHappensBefore(0, 0, annData);
    myHappensBeforeSync(annData);
}

//=============================
// happensAfterSync
//=============================
void AppThrAnn::happensAfterSync() {
    AnnData* annData = new AnnData();
    myHappensAfterSync(annData);
    myTSanMod->annotateHappensAfter(0, 0, annData);
}


//=============================
// genTLCAddr
//=============================
void* AppThrAnn::genTLCAddr() {
    //return new AnnData();
    AnnData* addr = new AnnData();
    myTSanMod->annotateHappensBefore(0, 0, addr);
    return addr;
}

//=============================
// translateRank
//=============================
int AppThrAnn::translateRank (I_Comm* comm, int rank)
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
// Destructor.
//=============================
AppThrAnn::~AppThrAnn (void)
{
    // clean up lock map
    for (std::map<MustWinType, AnnData*>::iterator it = myRWLockMap.begin();
            it != myRWLockMap.end(); ++it)
        delete it->second;
    myRWLockMap.clear();

    if (myTSanMod)
        destroySubModuleInstance ((I_Module*) myTSanMod);
    myTSanMod = NULL;
}
