/* This file is part of GTI (Generic Tool Infrastructure)
 *
 * Copyright (C)
 *  2008-2019 ZIH, Technische Universitaet Dresden, Federal Republic of Germany
 *  2008-2019 Lawrence Livermore National Laboratories, United States of America
 *  2013-2019 RWTH Aachen University, Federal Republic of Germany
 *
 * See the LICENSE file in the package base directory for details
 */

/**
 * @file VectorClock.cpp
 *       @see I_VectorClock.
 *
 *  @date 26.05.2021
 *  @author Felix Tomski
 */

#include "GtiApi.h"
#include "GtiMacros.h"
#include "I_CommProtocol.h"

#include "VectorClock.h"
#include <sys/resource.h>
#include <tuple>
#include <utility>
#include <vector>

#include "BinomialTree.h"
#include "Clock.h"

#ifdef GTI_DEBUG
#define VC_DEBUG 2
#endif

using namespace gti;

mGET_INSTANCE_FUNCTION(VectorClock);
mFREE_INSTANCE_FUNCTION(VectorClock);
mPNMPI_REGISTRATIONPOINT_FUNCTION(VectorClock);

//=============================
// Constructor
//=============================
VectorClock::VectorClock(const char* instanceName)
    : gti::ModuleBase<VectorClock, I_VectorClock>(instanceName), myNumProcs(0), id(-1), myClock(),
      clockQueues(), responseClocks(), lockClocks(), myPlaceMod(nullptr), myP2pIntraLayerTime(0) {
    // create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances();

    // handle sub modules
#define NUM_SUBS 3
    /* if (subModInstances.size() < NUM_SUBS) {
        std::cerr << "Module has not enough sub modules, check its analysis specification !(" <<
    __FILE__ << " @" << __LINE__ << ") " << std::endl; assert(0);
    } */
    if (subModInstances.size() > NUM_SUBS) {
        for (int i = NUM_SUBS; i < subModInstances.size(); i++)
            destroySubModuleInstance(subModInstances[i]);
    }

    // Initialize module data
    getWrapAcrossFunction("passVClockAcrossP2P", (GTI_Fct_t*)&myPassVClockAcrossP2PFunc);
    getWrapAcrossFunction("passUnlockClockToProxy", (GTI_Fct_t*)&myPassUnlockClockProxyFunc);
    getWrapAcrossFunction("passUnlockClockToEnd", (GTI_Fct_t*)&myPassUnlockClockEndFunc);
    getWrapAcrossFunction("passLockNotify", (GTI_Fct_t*)&myPassLockNotifyFunc);
    getWrapperFunction("syncNotify", (GTI_Fct_t*)&mySyncNotifyFunc);

    std::map<std::string, std::string> data = getData();

    if (data.find("id") != data.end())
        id = stoi(data["id"]);
    else {
        std::cerr << "[VClock] error: Could not find 'id' field in getData()" << std::endl;
        assert(0);
    }

    if (data.find("gti_level_1_size") != data.end()) {
        myNumProcs = stoi(data["gti_level_1_size"]);
        myClock.resize(myNumProcs);
        myClock.setOwnerId(id);
        // TODO: fix iterator
        myClock.fill(0);
    } else {
        std::cerr << "[VClock] error: Could not find 'gti_level_1_size' field in "
                     "getData()"
                  << std::endl;
        assert(0);
    }

    const char* COLL_STRAT = std::getenv("VC_STRAT");
    myCollStratMod = (I_CollStrat*)subModInstances[static_cast<int>(strToSColltrategy(COLL_STRAT))];
}

//=============================
// Destructor
//=============================
VectorClock::~VectorClock() {
    /* Collect time spent in collectives and P2P analysis functions */
#ifdef VC_DEBUG >= 3
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

    auto myCollIntraLayerTime = myCollStratMod->getCollIntraLayerTime();
    printf("IntraLayerP2pTime: %lu\nIntraLayerCollTime: %lu\n", myP2pIntraLayerTime, myCollStratMod->getCollIntraLayerTime()); 
    if (rank == 0) {
        std::vector<uint64_t> p2pTimes;
        std::vector<uint64_t> collTimes;
        p2pTimes.resize(size);
        collTimes.resize(size);
        PMPI_Gather(&myP2pIntraLayerTime, 1, MPI_UINT64_T, p2pTimes.data(), 1, MPI_UINT64_T, 0, myToolComm);
        PMPI_Gather(&myCollIntraLayerTime, 1, MPI_UINT64_T, collTimes.data(), 1, MPI_UINT64_T, 0, myToolComm);


        printf("IntraLayerP2pTime: %lu %lu %f\nIntraLayerCollTime: %lu %lu %f\n",
                *std::min_element(p2pTimes.begin(), p2pTimes.end()), *std::max_element(p2pTimes.begin(), p2pTimes.end()), 
                std::accumulate(p2pTimes.begin(), p2pTimes.end(), 0.0) / size,
                *std::min_element(collTimes.begin(), collTimes.end()), *std::max_element(collTimes.begin(), collTimes.end()), 
                std::accumulate(collTimes.begin(), collTimes.end(), 0.0) / size);

    } else {
        PMPI_Gather(&myP2pIntraLayerTime, 1, MPI_UINT64_T, NULL, 0, MPI_UINT64_T, 0, myToolComm);
        PMPI_Gather(&myCollIntraLayerTime, 1, MPI_UINT64_T, NULL, 0, MPI_UINT64_T, 0, myToolComm);
    }
#endif

    if (myCollStratMod)
        destroySubModuleInstance((I_Module*)myCollStratMod);
    myCollStratMod = NULL;

#if VC_DEBUG >= 1
    //    printf("[VClock] shutdown(%d): %s\n", id, clockToStr().c_str());
    struct rusage usage;
    int res = getrusage(RUSAGE_SELF, &usage);
    printf("[VClock] shutdown(%d): mem=%ld, clk=%s\n", id, usage.ru_maxrss, clockToStr().c_str());
#endif
}

//=============================
// init
//=============================
GTI_ANALYSIS_RETURN VectorClock::init() {
    getPlaceMod(&myPlaceMod);

#if VC_DEBUG >= 2
    printf("[VClock] init(%d): %s\n", id, clockToStr().c_str());
#endif

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// tick
//=============================
GTI_ANALYSIS_RETURN VectorClock::tick() {
#if VC_DEBUG >= 1
    if (id < 0 || id >= myClock.size()) {
        std::cerr << "[VClock] error: Invalid rank (" << id << ") for tick()" << std::endl;
        return GTI_ANALYSIS_FAILURE;
    }
#endif

    if (!myPlaceMod)
        getPlaceMod(&myPlaceMod);
    myClock[id] += 1;

#if VC_DEBUG >= 1
    printf("[VClock] tick(%d): %s\n", id, clockToStr().c_str());
#endif

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// bufferSignal
//=============================
GTI_ANALYSIS_RETURN VectorClock::bufferSignal(AppId appRank, QueueId queueId,
                                              RequestId requestHandle) {
    GtiId targetPlaceId;
    getLevelIdForApplicationRank(appRank, &targetPlaceId);

#if VC_DEBUG >= 2
    printf("[VClock] Buffering VClock %d to %d under handle %lu\n", id, targetPlaceId,
           requestHandle);
#endif

    requestTypeInfos.emplace(requestHandle, RequestType::send);
    signalInfos.emplace(std::piecewise_construct, std::forward_as_tuple(requestHandle),
                        std::forward_as_tuple(myClock, true, targetPlaceId, queueId));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addPersistentSendInfo
//=============================
GTI_ANALYSIS_RETURN VectorClock::addPersistentSendInfo(AppId appRank, QueueId queueId,
                                                       RequestId requestHandle) {
    GtiId targetPlaceId;
    getLevelIdForApplicationRank(appRank, &targetPlaceId);

#if VC_DEBUG >= 2
    printf("[VClock] Buffering VClock %d to %d under handle %lu\n", id, targetPlaceId,
           requestHandle);
#endif

    requestTypeInfos.emplace(requestHandle, RequestType::persistent_send);
    myPersistentSendInfos.emplace(std::piecewise_construct, std::forward_as_tuple(requestHandle),
                                  std::forward_as_tuple(targetPlaceId, queueId, false));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addPersistentRecvInfo
//=============================
GTI_ANALYSIS_RETURN VectorClock::addPersistentRecvInfo(RequestId requestHandle, uint64_t comm) {
    requestTypeInfos.emplace(requestHandle, RequestType::persistent_recv);
    myPersistentRecvInfos.emplace(std::piecewise_construct, std::forward_as_tuple(requestHandle),
                                  std::forward_as_tuple(comm, false));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// bufferWait
//=============================
GTI_ANALYSIS_RETURN VectorClock::bufferWait(QueueId queueId, RequestId requestHandle) {
#if VC_DEBUG >= 2
    printf("[VClock] Buffering Wait %d under handle %lu\n", id, requestHandle);
#endif

    requestTypeInfos.emplace(requestHandle, RequestType::receive);
    waitInfos.emplace(requestHandle, queueId);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// cantor_pairing
//=============================
inline VectorClock::QueueId cantor_pairing(uint64_t comm, int tag) {
    return ((comm + tag) * (comm + tag + 1)) / 2 + tag;
}

//=============================
// handleRequest
//=============================
GTI_ANALYSIS_RETURN VectorClock::handleRequest(RequestId requestHandle, AppId source, int tag) {
    auto info = requestTypeInfos.find(requestHandle);
    if (info == requestTypeInfos.end()) {
#if VC_DEBUG >= 1
        printf("[VClock] %d could not handle request %lu\n", id, requestHandle);
#endif
        /* This is actually ok for handles corresponding to signals (e.g. MPI_Isend), because we
         * don't buffer signals */
        return GTI_ANALYSIS_SUCCESS;
    }

    GTI_ANALYSIS_RETURN returnVal = GTI_ANALYSIS_SUCCESS;

    switch (info->second) {
    case RequestType::send: {
        auto sInfo = signalInfos.find(requestHandle);
        returnVal = sendBufferedSignal(sInfo->second);
        signalInfos.erase(sInfo);
        break;
    }
    case RequestType::persistent_send: {
        auto sInfo = myPersistentSendInfos.find(requestHandle);
        if (!sInfo->second.isActive)
            returnVal = signal(0, sInfo->second.remotePlaceId, sInfo->second.queueId);
        sInfo->second.isActive = !sInfo->second.isActive;
        break;
    }
    case RequestType::receive: {
        auto wInfo = waitInfos.find(requestHandle);
        returnVal = waitForSignal(source, cantor_pairing(wInfo->second, tag));
        waitInfos.erase(wInfo);
        break;
    }
    case RequestType::persistent_recv: {
        auto& wInfo = myPersistentRecvInfos.find(requestHandle)->second;
        if (wInfo.isActive)
            returnVal = waitForSignal(source, cantor_pairing(wInfo.comm, tag));
        wInfo.isActive = !wInfo.isActive;
        break;
    }
    case RequestType::alltoall: {
        auto cInfo = collInfos.find(requestHandle);
        returnVal =
            internalA2a(cInfo->second.groupRanks, cInfo->second.queueId, cInfo->second.clock,
                        cInfo->second.localRank, cInfo->second.localRoot, cInfo->second.worldRoot);
        collInfos.erase(cInfo);
        break;
    }
    case RequestType::alltoone: {
        auto cInfo = collInfos.find(requestHandle);
        /* Root does not need to buffer its clock */
        GtiId rootId;
        getLevelIdForApplicationRank(cInfo->second.worldRoot, &rootId);
        if (id != rootId)
            returnVal = internalA2o(cInfo->second.groupRanks, cInfo->second.queueId,
                                    cInfo->second.clock, cInfo->second.localRank,
                                    cInfo->second.localRoot, cInfo->second.worldRoot);
        else
            returnVal = internalA2o(cInfo->second.groupRanks, cInfo->second.queueId, myClock,
                                    cInfo->second.localRank, cInfo->second.localRoot,
                                    cInfo->second.worldRoot);
        collInfos.erase(cInfo);
        break;
    }
    case RequestType::onetoall: {
        auto cInfo = collInfos.find(requestHandle);
        /* Root does not need to buffer its clock */
        GtiId rootId;
        getLevelIdForApplicationRank(cInfo->second.worldRoot, &rootId);
        if (id == rootId)
            returnVal = internalO2a(cInfo->second.groupRanks, cInfo->second.queueId,
                                    cInfo->second.clock, cInfo->second.localRank,
                                    cInfo->second.localRoot, cInfo->second.worldRoot);
        else
            returnVal = internalO2a(cInfo->second.groupRanks, cInfo->second.queueId, myClock,
                                    cInfo->second.localRank, cInfo->second.localRoot,
                                    cInfo->second.worldRoot);
        collInfos.erase(cInfo);
        break;
    }
    }

    requestTypeInfos.erase(info);

    return returnVal;
}

//=============================
// sendBufferedSignal
//=============================
GTI_ANALYSIS_RETURN
VectorClock::sendBufferedSignal(const SignalRequestInfo& info) {
#if VC_DEBUG >= 1
    if (!myPassVClockAcrossP2PFunc) {
        std::cerr << "[VClock] error: myPassVClockAcrossFunc undefined" << std::endl;
        return GTI_ANALYSIS_FAILURE;
    }
#endif

#if VC_DEBUG >= 2
    printf("[VClock] %d sending buffered clock to %d\n", id, info.remotePlaceId);
#endif
    (*myPassVClockAcrossP2PFunc)(info.clock.data(), myClock.size(), id, info.isSync, 0,
                                 info.queueId, info.remotePlaceId);

    // TODO: fix passing id instead of app rank
    return waitForResponse(static_cast<int>(info.remotePlaceId), info.queueId);
}

//=============================
// signal
//=============================
GTI_ANALYSIS_RETURN VectorClock::signal(int isSync, AppId appRank, QueueId queueId) {
#if VC_DEBUG >= 1
    if (!myPassVClockAcrossP2PFunc) {
        std::cerr << "[VClock] error: myPassVClockAcrossFunc undefined" << std::endl;
        return GTI_ANALYSIS_FAILURE;
    }
#endif

    GtiId targetPlaceId;
    getLevelIdForApplicationRank(appRank, &targetPlaceId);
#if VC_DEBUG >= 2
    printf("[VClock] Passing VClock from %d to %d under %lu, sync=%d\n", id, targetPlaceId, queueId, isSync);
#endif
    (*myPassVClockAcrossP2PFunc)(myClock.data(), myClock.size(), id, isSync, 0, queueId,
                                 targetPlaceId);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// waitForResponse
//=============================
GTI_ANALYSIS_RETURN VectorClock::waitForResponse(AppId appRank, QueueId queueId) {
    GtiId originId;
    getLevelIdForApplicationRank(appRank, &originId);

    auto responseClock = responseClocks[originId].find(queueId);

    auto startTime = getUsecTime();
    while (responseClock == responseClocks[originId].end()) {
        myPlaceMod->testIntralayer();
        responseClock = responseClocks[originId].find(queueId);
    }
    myP2pIntraLayerTime += getUsecTime() - startTime;

    myClock.merge(responseClock->second);
#if VC_DEBUG >= 2
    std::cout << "[VClock] " << id << " merged clock " << responseClock->second.toStr() << " from "
              << originId << " directly to " << myClock.toStr() << std::endl;
#endif
    responseClocks[originId].erase(responseClock);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// receiveVClockP2P
//=============================
GTI_ANALYSIS_RETURN VectorClock::receiveVClockP2P(ClockEntry* vectorClock, GtiId originId,
                                                  int isSync, int isResponse, QueueId queueId) {
#if VC_DEBUG >= 1
    if (id == originId) {
        std::cerr << "[VClock] error: receiveClock() called on own id: " << id << std::endl;
        return GTI_ANALYSIS_FAILURE;
    }

    if (originId >= myClock.size()) {
        std::cerr << "[VClock] error: receiveClock() on " << id << " from invalid id: " << originId
                  << std::endl;
        return GTI_ANALYSIS_FAILURE;
    }
#endif

    /* Gets merged in waitForResponse */
    if (isResponse) {
        responseClocks[originId].emplace(std::piecewise_construct, std::forward_as_tuple(queueId),
                                         std::forward_as_tuple(vectorClock, myNumProcs, originId));
        return GTI_ANALYSIS_SUCCESS;
    }

    auto clockQueue = clockQueues[originId].find(queueId);
    if (clockQueue == clockQueues[originId].end()) {
        clockQueue = clockQueues[originId]
                         .emplace(std::piecewise_construct, std::forward_as_tuple(queueId),
                                  std::forward_as_tuple())
                         .first;
#if VC_DEBUG >= 2
        printf("[VClock] %d created empty queue through %d under %lu\n", id, originId, queueId);
#endif
    }

    /* Buffer clock under queue handle, gets merged in in waitForSignal */
    clockQueue->second.emplace(Clock(vectorClock, myNumProcs, originId), isSync, originId);
#if VC_DEBUG >= 2
    printf("[VClock] %d buffered clock from %d under %lu : %s, queue size: %lu \n", id, originId,
           queueId, clockQueue->second.front().clock.toStr().c_str(), clockQueue->second.size());
#endif

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// waitForSignal
//=============================
GTI_ANALYSIS_RETURN VectorClock::waitForSignal(AppId originAppRank, QueueId queueId) {
    GtiId originId;
    getLevelIdForApplicationRank(originAppRank, &originId);
#if VC_DEBUG >= 2
    printf("[VClock] %d waitingForSignal from %d under %lu\n", id, originId, queueId);
#endif

    auto peerIt = clockQueues[originId];
    /* Wait for the clock of the communication context (queueId) to arrive */
    auto clockQueue = peerIt.find(queueId);
    if (clockQueue == peerIt.end())
        clockQueue = peerIt
                         .emplace(std::piecewise_construct, std::forward_as_tuple(queueId),
                                  std::forward_as_tuple())
                         .first;
    auto startTime = getUsecTime();
    while (clockQueues[originId][queueId].empty()) {
        myPlaceMod->testIntralayer();
    myP2pIntraLayerTime += getUsecTime() - startTime;

#if VC_DEBUG >= 3
        printf("[VClock] %d waitingForSignal from %d under %lu (polling)\n", id, originId, queueId);
#endif
    }

    auto tmpClockContext = clockQueues[originId][queueId].front();
#if VC_DEBUG >= 2
    printf("[VClock] %d merging %s with %s from %d\n", id, clockToStr().c_str(),
           tmpClockContext.clock.toStr().c_str(), originId);
#endif

    // mentioned in Fidge's paper because of overtaking
    if (!tmpClockContext.isSync) {
        if (myClock[originId] <= tmpClockContext.clock[originId])
            myClock[originId] += 1;
    }
    /* Send back own vector clock in case of synchronous communication. */
    else if (tmpClockContext.isSync) {
#if VC_DEBUG >= 2
        std::cout << "Passing receivers clock over from " << id << " to " << originId << std::endl;
#endif
        (*myPassVClockAcrossP2PFunc)(myClock.data(), myClock.size(), id, true, true, queueId,
                                     originId);
    }
    mergeClock(tmpClockContext.clock);
    clockQueues[originId][queueId].pop();

#if VC_DEBUG >= 2
    printf("[VClock] %d merged clock with %d clock to %s\n", id, originId, clockToStr().c_str());
#endif

    return GTI_ANALYSIS_SUCCESS;
}

//
//=============================
// internalA2o
//=============================
GTI_ANALYSIS_RETURN VectorClock::allToOne(const std::vector<int>& groupRanks, QueueId queueId,
                                          AppId localRank, AppId localRoot, AppId worldRoot) {

    Clock newClock(myClock);
    auto res = internalA2o(groupRanks, queueId, newClock, localRank, localRoot, worldRoot);
    mergeClock(newClock);

    return res;
}

//=============================
// allToOne
//=============================
GTI_ANALYSIS_RETURN VectorClock::internalA2o(const std::vector<int>& groupRanks, QueueId queueId,
                                             Clock& clockToUse, AppId localRank, AppId localRoot,
                                             AppId worldRoot) {
    GTI_ANALYSIS_RETURN res =
        myCollStratMod->reduce(clockToUse.data(), clockToUse.data(), clockToUse.size(), localRoot,
                               localRank, groupRanks, queueId);
    /* For non-blocking, we first merge on the buffered clock */
    if (localRank == localRoot && &clockToUse != &myClock)
        mergeClock(clockToUse);

    return res;
}

//=============================
// oneToAll
//=============================
GTI_ANALYSIS_RETURN VectorClock::oneToAll(AppId localRank, AppId localRoot, AppId worldRoot,
                                          const std::vector<int>& groupRanks, QueueId queueId) {
    Clock newClock(myClock);
    auto res = internalO2a(groupRanks, queueId, newClock, localRank, localRoot, worldRoot);
    mergeClock(newClock);

    return res;
}

//=============================
// oneToAll
//=============================
GTI_ANALYSIS_RETURN VectorClock::internalO2a(const std::vector<int>& groupRanks, QueueId queueId,
                                             Clock& clockToUse, AppId localRank, AppId localRoot,
                                             AppId worldRoot) {
    return myCollStratMod->broadcast(clockToUse.data(), clockToUse.size(), localRoot, localRank,
                                     groupRanks, queueId);
}

//=============================
// bufferA2aClock
//=============================
GTI_ANALYSIS_RETURN
VectorClock::bufferA2aClock(const std::vector<int>& groupRanks, QueueId queueId, RequestId request,
                            AppId localRank, AppId localRoot, AppId worldRoot) {
    requestTypeInfos.emplace(request, RequestType::alltoall);
    collInfos.emplace(
        std::piecewise_construct, std::forward_as_tuple(request),
        std::forward_as_tuple(myClock, groupRanks, localRank, localRoot, worldRoot, queueId));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// bufferA2oClock
//=============================
GTI_ANALYSIS_RETURN
VectorClock::bufferA2oClock(const std::vector<int>& groupRanks, QueueId queueId, RequestId request,
                            AppId localRank, AppId localRoot, AppId worldRoot) {
    requestTypeInfos.emplace(request, RequestType::alltoone);
    /* Root doesn't need to buffer its clock */
    GtiId rootId;
    getLevelIdForApplicationRank(worldRoot, &rootId);
    if (id != rootId)
        collInfos.emplace(
            std::piecewise_construct, std::forward_as_tuple(request),
            std::forward_as_tuple(myClock, groupRanks, localRank, localRoot, worldRoot, queueId));
    else
        collInfos.emplace(
            std::piecewise_construct, std::forward_as_tuple(request),
            std::forward_as_tuple(myClock, groupRanks, localRank, localRoot, worldRoot, queueId));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// bufferO2aClock
//=============================
GTI_ANALYSIS_RETURN
VectorClock::bufferO2aClock(const std::vector<int>& groupRanks, QueueId queueId, RequestId request,
                            AppId localRank, AppId localRoot, AppId worldRoot) {
    requestTypeInfos.emplace(request, RequestType::alltoone);
    /* Only root needs to save its clock, but other processes also need group
     * and rank infos */
    GtiId rootId;
    getLevelIdForApplicationRank(worldRoot, &rootId);
    if (id == rootId)
        collInfos.emplace(
            std::piecewise_construct, std::forward_as_tuple(request),
            std::forward_as_tuple(myClock, groupRanks, localRank, localRoot, worldRoot, queueId));
    else
        collInfos.emplace(
            std::piecewise_construct, std::forward_as_tuple(request),
            std::forward_as_tuple(Clock(), groupRanks, localRank, localRoot, worldRoot, queueId));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// allToAll
//=============================
GTI_ANALYSIS_RETURN VectorClock::allToAll(const std::vector<int>& groupRanks, QueueId queueId,
                                          AppId localRank, AppId localRoot, AppId worldRoot) {
    Clock newClock(myClock);
    auto res = internalA2a(groupRanks, queueId, newClock, localRank, localRoot, worldRoot);
    mergeClock(newClock);

    return res;
}

//=============================
// internal_A2a
//=============================
GTI_ANALYSIS_RETURN VectorClock::internalA2a(const std::vector<int>& groupRanks, QueueId queueId,
                                             Clock& initClock, AppId localRank, AppId localRoot,
                                             AppId worldRoot) {
    auto res = myCollStratMod->allreduce(initClock.data(), initClock.data(), initClock.size(),
                                         localRank, groupRanks, queueId);
    /* For non-blocking, we first merge on the buffered clock */
    if (&initClock != &myClock)
        mergeClock(initClock);

    return res;
}

//=============================
// bufferUnlockClock
//=============================
GTI_ANALYSIS_RETURN VectorClock::bufferUnlockClock(ClockEntry* lockClock, size_t clockSize,
                                                   LockId lockHandle, GtiId originId) {
#if VC_DEBUG >= 2
    printf("[VClock] %d buffering unlock clock %s under %lu from %d\n", id,
           Clock(lockClock, clockSize, originId).toStr().c_str(), lockHandle, originId);
#endif
    auto it = lockClocks.find(lockHandle);
    if (it == lockClocks.end()) {
        lockClocks.insert(
            {lockHandle,
             std::pair<bool, ClockContext>(
                 {false, ClockContext(Clock(lockClock, clockSize, originId), 0, originId)})});
    } else {
        std::memcpy(it->second.second.clock.data(), lockClock, sizeof(ClockEntry) * clockSize);
        it->second.first = false;
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// unlock
//=============================
GTI_ANALYSIS_RETURN VectorClock::unlock(LockId lockHandle, AppId appRank) {
    GtiId placeId;
    getLevelIdForApplicationRank(appRank, &placeId);
#if VC_DEBUG >= 2
    printf("[VClock] %d passing unlock clock %s under %lu to proxy %d\n", id, clockToStr().c_str(),
           lockHandle, placeId);
#endif
    myPassUnlockClockProxyFunc(myClock.data(), myClock.size(), lockHandle, id, placeId);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// lock
//=============================
GTI_ANALYSIS_RETURN VectorClock::lock(LockId lockHandle, AppId appRank) {
    GtiId placeId;
    getLevelIdForApplicationRank(appRank, &placeId);
#if VC_DEBUG >= 2
    printf("[VClock] %d passing lock clock %s under %lu to proxy %d\n", id, clockToStr().c_str(),
           lockHandle, placeId);
#endif
    myPassLockNotifyFunc(lockHandle, id, placeId);

    auto it = waitingForUnlockClock.emplace(lockHandle, true).first;

    auto startTime = getUsecTime();
    /* Wait for the clock of the process that releases the lock, gets directly merged in recvUnlockClock() */
    do {
        myPlaceMod->testIntralayer();
    } while (waitingForUnlockClock[lockHandle]);
    myP2pIntraLayerTime += getUsecTime() - startTime;
    waitingForUnlockClock.erase(lockHandle);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// handleLockNotify
//=============================
GTI_ANALYSIS_RETURN VectorClock::handleLockNotify(LockId lockHandle, GtiId originId) {
#if VC_DEBUG >= 2
    printf("[VClock] %d trying to handle lock notification under %lu from %d\n", id, lockHandle,
           originId);
#endif

    auto it = lockClocks.find(lockHandle);
    /* Insert default clock of all 0's for first lock request */
    if (it == lockClocks.end()) {
        myPassUnlockClockEndFunc(NULL, 0, lockHandle, 0, originId);
        lockClocks.insert(
            {lockHandle, std::pair<bool, ClockContext>(
                             {true, ClockContext(Clock(myClock.size(), id), 0, originId)})});

#if VC_DEBUG >= 2
        printf("[VClock] %d handling lock notification under %lu from %d: sent "
               "empty clock\n",
               id, lockHandle, originId);
#endif
    } else {
        /* Wait for unlock */
        auto startTime = getUsecTime();
        while (lockClocks[lockHandle].first) {
            myPlaceMod->testIntralayer();
        }
        myP2pIntraLayerTime += getUsecTime() - startTime;

        /* Forward the clock received from the unlocking process to the one that just locked */
        myPassUnlockClockEndFunc(it->second.second.clock.data(), it->second.second.clock.size(),
                                 lockHandle, it->second.second.remotePlaceId, originId);
#if VC_DEBUG >= 2
        printf("[VClock] %d handling lock notification under %lu from %d: sent "
               "clock%s\n",
               id, lockHandle, originId, it->second.second.clock.toStr().c_str());
#endif
        lockClocks.erase(it);
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvUnlockClock
//=============================
GTI_ANALYSIS_RETURN VectorClock::recvUnlockClock(ClockEntry* lockClock, size_t clockSize,
                                                 LockId lockHandle, GtiId originId) {
#if VC_DEBUG >= 2
    printf("[VClock] %d received unlock clock %s under %lu from %d\n", id,
           Clock(lockClock, clockSize, originId).toStr().c_str(), lockHandle, originId);
#endif
    auto it = waitingForUnlockClock.find(lockHandle);
    it->second = false;
    if (lockClock)
        mergeClock(GtiClock(lockClock, myClock.size(), originId));
    return GTI_ANALYSIS_SUCCESS;
}


//=============================
// mergeClock
//=============================
void VectorClock::mergeClock(const AbstractClock& other) {
    // Check for changes in clock before merge and notify other modules
    if (mySyncNotifyFunc) {
        for (int i = 0; i < myClock.size(); i++) {
            if (i != id && myClock.data()[i] < other.data()[i])
                mySyncNotifyFunc(i, id);
        }
    }

    myClock.merge(other);
}