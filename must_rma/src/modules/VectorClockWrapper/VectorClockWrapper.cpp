/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file VectorClockWrapper.cpp
 *       @see I_VectorClockWrapper.h.
 *
 *  @date 29.06.2021
 *  @author Felix Tomski
 */

#include "GtiMacros.h"
#include "VectorClockWrapper.h"

using namespace must;

mGET_INSTANCE_FUNCTION(VectorClockWrapper) mFREE_INSTANCE_FUNCTION(VectorClockWrapper)
    mPNMPI_REGISTRATIONPOINT_FUNCTION(VectorClockWrapper)

    //=============================
    // Constructor
    //=============================
    VectorClockWrapper::VectorClockWrapper(const char* instanceName)
    : gti::ModuleBase<VectorClockWrapper, I_VectorClockWrapper>(instanceName), myP2pTime(0), myCollTime(0) {
    // create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances();

    // handle sub modules
#define NUM_SUBMODULES 4
    if (subModInstances.size() < NUM_SUBMODULES) {
        std::cerr << "Module has not enough sub modules, check its analysis specification! ("
                  << __FILE__ << "@" << __LINE__ << ")" << std::endl;
        assert(0);
    }
    if (subModInstances.size() > NUM_SUBMODULES) {
        for (std::vector<I_Module*>::size_type i = NUM_SUBMODULES; i < subModInstances.size(); i++)
            destroySubModuleInstance(subModInstances[i]);
    }

    myVectorClockMod = (I_VectorClock*)subModInstances[0];
    myCommTrackMod = (I_CommTrack*)subModInstances[1];
    myGroupTrackMod = (I_GroupTrack*)subModInstances[2];
    myWinTrackMod = (I_WinTrack*)subModInstances[3];

    // Initialize module data
    /*None needed*/
}

//=============================
// Destructor
//=============================
VectorClockWrapper::~VectorClockWrapper() {
    PNMPI_modHandle_t handle;
    PNMPI_Service_descriptor_t service;
    PNMPI_Service_Fct_t fct;
    MPI_Group myToolGroup;
    MPI_Comm myToolComm;

    int err = PNMPI_Service_GetModuleByName("split_processes", &handle);
    assert(err == PNMPI_SUCCESS);
    err = PNMPI_Service_GetServiceByName(handle, "SplitMod_getMySetComm", "p", &service);
    assert(err == PNMPI_SUCCESS);
    MPI_Comm fakeComm;
    ((int (*)(void*))service.fct)(&fakeComm);
    XMPI_Comm_dup(fakeComm, &myToolComm);
    XMPI_Comm_group(myToolComm, &myToolGroup);

    int rank, size;
    PMPI_Comm_rank(myToolComm, &rank);
    PMPI_Comm_size(myToolComm, &size);

//    printf("VectorClockP2pTime: %lu\nVectorClockCollTime: %lu\n", myP2pTime, myCollTime); 
    if (rank == 0) {
        std::vector<uint64_t> p2pTimes;
        std::vector<uint64_t> collTimes;
        p2pTimes.resize(size);
        collTimes.resize(size);
        PMPI_Gather(&myP2pTime, 1, MPI_UINT64_T, p2pTimes.data(), 1, MPI_UINT64_T, 0, myToolComm);
        PMPI_Gather(&myCollTime, 1, MPI_UINT64_T, collTimes.data(), 1, MPI_UINT64_T, 0, myToolComm);

        /* printf("VClockP2pTime: %lu %lu %f\nVClockCollTime: %lu %lu %f\n",
                *std::min_element(p2pTimes.begin(), p2pTimes.end()), *std::max_element(p2pTimes.begin(), p2pTimes.end()), 
                std::accumulate(p2pTimes.begin(), p2pTimes.end(), 0.0) / size,
                *std::min_element(collTimes.begin(), collTimes.end()), *std::max_element(collTimes.begin(), collTimes.end()), 
                std::accumulate(collTimes.begin(), collTimes.end(), 0.0) / size);*/

    } else {
        PMPI_Gather(&myP2pTime, 1, MPI_UINT64_T, NULL, 0, MPI_UINT64_T, 0, myToolComm);
        PMPI_Gather(&myCollTime, 1, MPI_UINT64_T, NULL, 0, MPI_UINT64_T, 0, myToolComm);
    }

    if (myVectorClockMod)
        destroySubModuleInstance((I_Module*)myVectorClockMod);
    myVectorClockMod = NULL;

    if (myCommTrackMod)
        destroySubModuleInstance((I_Module*)myCommTrackMod);
    myCommTrackMod = NULL;

    if (myGroupTrackMod)
        destroySubModuleInstance((I_Module*)myGroupTrackMod);
    myGroupTrackMod = NULL;

    if (myWinTrackMod)
        destroySubModuleInstance((I_Module*)myWinTrackMod);
    myWinTrackMod = NULL;

    assert(sharedLocks.empty());
}

//=============================
// tick
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::tick() { return myVectorClockMod->tick(); }

//=============================
// init
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::init() { return myVectorClockMod->init(); }

inline std::uint64_t cantor_pairing(uint64_t comm, int tag) {
    return ((comm + tag) * (comm + tag + 1)) / 2 + tag;
}

//=============================
// signal
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::signal(MustParallelId pId, int isSync, int appRank, int tag,
                                               MustCommType comm) {
    auto startTime = getUsecTime();
    auto res = myVectorClockMod->signal(
        isSync, appRank, cantor_pairing(myCommTrackMod->getComm(pId, comm)->getContextId(), tag));
    myP2pTime += getUsecTime() - startTime;
    return res;
}

//=============================
// wait
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::wait(MustParallelId pId, int appRank, int tag,
                                             MustCommType comm) {
    auto startTime = getUsecTime();
    auto res = myVectorClockMod->waitForSignal(
        appRank, cantor_pairing(myCommTrackMod->getComm(pId, comm)->getContextId(), tag));
    myP2pTime += getUsecTime() - startTime;
    return res;
}

//=============================
// waitForResponse
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::waitForResponse(MustParallelId pId, int appRank, int tag,
                                                        MustCommType comm) {
    auto startTime = getUsecTime();
    auto res = myVectorClockMod->waitForResponse(
        appRank, cantor_pairing(myCommTrackMod->getComm(pId, comm)->getContextId(), tag));
    myP2pTime += getUsecTime() - startTime;
    return res;
}

//=============================
// bufferWait
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::bufferWait(MustParallelId pId, MustCommType comm,
                                                   MustRequestType request) {
    auto startTime = getUsecTime();
    auto res = myVectorClockMod->bufferWait(myCommTrackMod->getComm(pId, comm)->getContextId(),
                                        request);
    myP2pTime += getUsecTime() - startTime;
    return res;
}

//=============================
// allToOne
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::allToOne(MustParallelId pId, MustCommType comm, int root) {
    auto startTime = getUsecTime();
    I_Comm* commInfo = myCommTrackMod->getComm(pId, comm);

    /* Ignore operations on intercommunicators for now */
    if (commInfo->isIntercomm())
        return gti::GTI_ANALYSIS_SUCCESS;

    int localRank, worldRoot;
    commInfo->getGroup()->containsWorldRank(myVectorClockMod->getId(), &localRank);
    commInfo->getGroup()->translate(root, &worldRoot);

    auto res = myVectorClockMod->allToOne(commInfo->getGroup()->getMapping(), commInfo->getContextId(),
                                      localRank, root, worldRoot);
    myCollTime += getUsecTime() - startTime;
    return res;
}

//=============================
// oneToAll
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::oneToAll(MustParallelId pId, MustCommType comm, int root) {
    auto startTime = getUsecTime();
    I_Comm *worldCommInfo, *commInfo = myCommTrackMod->getComm(pId, comm);

    /* Ignore operations on intercommunicators for now */
    if (commInfo->isIntercomm())
        return gti::GTI_ANALYSIS_SUCCESS;

    int localRank, worldRoot;
    commInfo->getGroup()->containsWorldRank(myVectorClockMod->getId(), &localRank);
    commInfo->getGroup()->translate(root, &worldRoot);

    auto res = myVectorClockMod->oneToAll(
        localRank, root, worldRoot, commInfo->getGroup()->getMapping(),
        commInfo->getContextId() // TODO: discuss if this is sufficient (hash groupTable?)
    );
    myCollTime += getUsecTime() - startTime;
    return res;
}

//=============================
// allToAll
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::allToAll(MustParallelId pId, MustCommType comm) {
    auto startTime = getUsecTime();
    I_Comm* commInfo = myCommTrackMod->getComm(pId, comm);

    /* Ignore operations on intercommunicators for now */
    if (commInfo->isIntercomm())
        return gti::GTI_ANALYSIS_SUCCESS;

    int localRank, worldRoot, localRoot = 0;
    commInfo->getGroup()->containsWorldRank(myVectorClockMod->getId(), &localRank);
    commInfo->getGroup()->translate(localRoot, &worldRoot);

    auto res = myVectorClockMod->allToAll(commInfo->getGroup()->getMapping(), commInfo->getContextId(),
                                      localRank, localRoot, worldRoot);
    myCollTime += getUsecTime() - startTime;
    return res;
}

//=============================
// bufferA2aClock
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::bufferA2aClock(MustParallelId pId, MustCommType comm,
                                                       MustRequestType request) {
    auto startTime = getUsecTime();
    I_Comm* commInfo = myCommTrackMod->getComm(pId, comm);

    /* Ignore operations on intercommunicators for now */
    if (commInfo->isIntercomm())
        return gti::GTI_ANALYSIS_SUCCESS;

    int localRank, worldRoot;
    commInfo->getGroup()->containsWorldRank(myVectorClockMod->getId(), &localRank);
    commInfo->getGroup()->translate(commInfo->getGroup()->getMapping()[0], &worldRoot);

    auto res = myVectorClockMod->bufferA2aClock(commInfo->getGroup()->getMapping(),
                                            commInfo->getContextId(), request, localRank,
                                            commInfo->getGroup()->getMapping()[0], worldRoot);
    myCollTime += getUsecTime() - startTime;
    return res;
}

//=============================
// bufferA2oClock
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::bufferA2oClock(MustParallelId pId, int root,
                                                       MustCommType comm, MustRequestType request) {
    auto startTime = getUsecTime();
    I_Comm* commInfo = myCommTrackMod->getComm(pId, comm);

    /* Ignore operations on intercommunicators for now */
    if (commInfo->isIntercomm())
        return gti::GTI_ANALYSIS_SUCCESS;

    int localRank, worldRoot;
    commInfo->getGroup()->containsWorldRank(myVectorClockMod->getId(), &localRank);
    commInfo->getGroup()->translate(root, &worldRoot);

    auto res = myVectorClockMod->bufferA2oClock(commInfo->getGroup()->getMapping(),
                                            commInfo->getContextId(), request, localRank, root,
                                            worldRoot);
    myCollTime += getUsecTime() - startTime;
    return res;
}

//=============================
// bufferA2oClock
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::bufferO2aClock(MustParallelId pId, int root,
                                                       MustCommType comm, MustRequestType request) {
    auto startTime = getUsecTime();
    I_Comm* commInfo = myCommTrackMod->getComm(pId, comm);

    /* Ignore operations on intercommunicators for now */
    if (commInfo->isIntercomm())
        return gti::GTI_ANALYSIS_SUCCESS;

    int localRank, worldRoot;
    commInfo->getGroup()->containsWorldRank(myVectorClockMod->getId(), &localRank);
    commInfo->getGroup()->translate(root, &worldRoot);

    auto res = myVectorClockMod->bufferA2oClock(commInfo->getGroup()->getMapping(),
                                            commInfo->getContextId(), request, localRank, root,
                                            worldRoot);
    myCollTime += getUsecTime() - startTime;
    return res;
}

//=============================
// handleRequest
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::handleRequest(MustRequestType request, int source,
                                                      int tag) {
    auto startTime = getUsecTime();
    auto res = myVectorClockMod->handleRequest(request, source, tag);
    myP2pTime += getUsecTime() - startTime;
    return res;
}

//=============================
// handleRequestArray
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::handleRequestArray(MustRequestType* requests, int* sources,
                                                           int* tags, size_t count) {
    auto startTime = getUsecTime();
    for (std::size_t i = 0; i < count; i++)
        myVectorClockMod->handleRequest(requests[i], sources[i], tags[i]);

    // TODO: return whether (at least) one failed?
    myP2pTime += getUsecTime() - startTime;
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// handleAnyRequest
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::handleAnyRequest(MustRequestType* requests, int index,
                                                         int source, int tag) {
    auto startTime = getUsecTime();
    auto res = myVectorClockMod->handleRequest(requests[index], source, tag);
    myP2pTime += getUsecTime() - startTime;
    return res;
}

//=============================
// winAllToAll
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::winAllToAll(MustParallelId pId, MustWinType winHandle) {
    return allToAll(pId, myWinTrackMod->getWin(pId, winHandle)->getCommHandle());
}

//=============================
// lock
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::lock(MustParallelId pId, MustWinType winHandle,
                                             int appRank, int lock_type) {
    // We ignore synchronization provided by shared locks, otherwise we might miss races.
    // TODO: Consider synchronization based on whether a lock is shared or not.
    if (lock_type == MPI_LOCK_SHARED) {
        sharedLocks.insert(std::make_pair(appRank, winHandle));
        return GTI_ANALYSIS_SUCCESS;
    } else {
        return myVectorClockMod->lock(myWinTrackMod->getWin(pId, winHandle)->getContextId(), appRank);
    }
}

//=============================
// unlock
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::unlock(MustParallelId pId, MustWinType winHandle,
                                               int appRank) {
    // We ignore synchronization provided by shared locks, otherwise we might miss races.
    // TODO: Consider synchronization based on whether a lock is shared or not.
    auto sharedLock = sharedLocks.find(std::make_pair(appRank, winHandle));
    if (sharedLock != sharedLocks.end()) {
        sharedLocks.erase(sharedLock);
        return GTI_ANALYSIS_SUCCESS;
    } else {
        return myVectorClockMod->unlock(myWinTrackMod->getWin(pId, winHandle)->getContextId(), appRank);
    }
}


//=============================
// winStart
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::winStart(MustParallelId pId, MustLocationId lId, MustGroupType groupHandle, MustWinType winHandle) {
    myWinCompleteGroupMap.insert(std::make_pair(winHandle, groupHandle));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winComplete
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::winComplete(MustParallelId pId, MustLocationId lId, MustWinType winHandle) {
    MustGroupType groupHandle = myWinCompleteGroupMap[winHandle];
    myWinCompleteGroupMap.erase(winHandle);
    for (auto const& rank : myGroupTrackMod->getGroup(pId, groupHandle)->getGroup()->getMapping()) {
        auto res = myVectorClockMod->signal(false, rank, myWinTrackMod->getWin(pId, winHandle)->getContextId());
        if (res != GTI_ANALYSIS_SUCCESS)
            return res;
    }

    return GTI_ANALYSIS_SUCCESS;
}


//=============================
// winPost
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::winPost(MustParallelId pId, MustLocationId lId, MustGroupType groupHandle, MustWinType winHandle) {
    myWinWaitGroupMap.insert(std::make_pair(winHandle, groupHandle));

    return GTI_ANALYSIS_SUCCESS;
}


//=============================
// winWait
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::winWait(MustParallelId pId, MustLocationId lId, MustWinType winHandle) {
    MustGroupType groupHandle = myWinWaitGroupMap[winHandle];
    myWinWaitGroupMap.erase(winHandle);
    for (auto const& rank : myGroupTrackMod->getGroup(pId, groupHandle)->getGroup()->getMapping()) {
        auto res = myVectorClockMod->waitForSignal(rank, myWinTrackMod->getWin(pId, winHandle)->getContextId());
        if (res != GTI_ANALYSIS_SUCCESS)
            return res;
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addPersistentSendInfo
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::addPersistentSendInfo(MustParallelId pId, int appRank,
                                                              int tag, MustRequestType request,
                                                              MustCommType comm) {
    auto startTime = getUsecTime();
    auto res = myVectorClockMod->addPersistentSendInfo(
        appRank, cantor_pairing(myCommTrackMod->getComm(pId, comm)->getContextId(), tag), request);
    myP2pTime += getUsecTime() - startTime;
    return res;
}
//
//=============================
// addPersistentRecvInfo
//=============================
GTI_ANALYSIS_RETURN VectorClockWrapper::addPersistentRecvInfo(MustParallelId pId,
                                                              MustRequestType request,
                                                              MustCommType comm) {
    auto startTime = getUsecTime();
    auto res = myVectorClockMod->addPersistentRecvInfo(
        request, myCommTrackMod->getComm(pId, comm)->getContextId());
    myP2pTime += getUsecTime() - startTime;
    return res;
}

/*EOF*/
