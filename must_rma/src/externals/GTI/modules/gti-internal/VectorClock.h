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
 * @file VectorClock.h
 *       @see I_VectorClock.
 *
 *  @date 26.05.2021
 *  @author Felix Tomski
 */

#include "I_CollStrat.h"
#include "I_VectorClock.h"
#include "ModuleBase.h"

#include "Clock.h"
#include "VectorClockApi.h"

#include <algorithm>
#include <cstring>
#include <limits.h> /* for CHAR_BIT */
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <sys/time.h>

#ifndef VECTORCLOCK_H
#define VECTORCLOCK_H

namespace gti {

class VectorClock : public ModuleBase<VectorClock, I_VectorClock> {
  public:
    class ClockContext;
    class CollRequestInfo;
    class SignalRequestInfo;
    class WaitRequestInfo;

    typedef unsigned long long ClockEntry;
    typedef uint64_t QueueId;
    typedef uint64_t LockId;
    typedef unsigned long RequestId;
    typedef int AppId; /* Needs to be translated by getLevelIdForApplicationRank() */
    typedef int GtiId;
    typedef std::queue<ClockContext> ClockQueue;

    /* Stores received clock and further information for P2P communication */
    class ClockContext {
      public:
        Clock clock;
        int isSync;
        GtiId remotePlaceId;

        ClockContext() : clock(), isSync(), remotePlaceId() {}

        ClockContext(Clock&& c, int s, GtiId placeId)
            : clock(std::forward<Clock>(c)), isSync(s), remotePlaceId(placeId) {}

        ClockContext(const Clock& c, int s, GtiId placeId)
            : clock(c), isSync(s), remotePlaceId(placeId) {}
    }; /*class ClockContext*/

    /* Request types for non-blocking communication which require a second call for completion */
    enum class RequestType {
        send,
        receive,
        alltoone,
        alltoall,
        onetoall,
        persistent_send,
        persistent_recv
    }; /*enum class RequestType*/

    enum class CollectiveType { alltoone, alltoall, onetoall };

    /* Stores context information for non-blocking collective communication */
    class CollRequestInfo {
      public:
        Clock clock; /* The local vector clock at communication initialization */
        const std::vector<int> groupRanks;
        AppId localRank;
        AppId localRoot;
        AppId worldRoot;
        QueueId queueId;
        CollRequestInfo(Clock&& c, const std::vector<int>& gr, AppId lrank, AppId lroot,
                        AppId wroot, QueueId q)
            : clock(std::forward<Clock>(c)), groupRanks(gr), localRank(lrank), localRoot(lroot),
              worldRoot(wroot), queueId(q){};

        CollRequestInfo(const Clock& c, const std::vector<int>& gr, AppId lrank, AppId lroot,
                        AppId wroot, QueueId q)
            : clock(c), groupRanks(gr), localRank(lrank), localRoot(lroot), worldRoot(wroot),
              queueId(q){};
    }; /*class CollRequestInfo*/

    /* Stores a signal for non-blocking P2P communication (currently unused) */
    class SignalRequestInfo {
      public:
        Clock clock;
        int isSync;
        GtiId remotePlaceId;
        QueueId queueId;
        SignalRequestInfo(Clock&& c, int s, GtiId r, QueueId q)
            : clock(std::forward<Clock>(c)), isSync(s), remotePlaceId(r), queueId(q){};

        SignalRequestInfo(const Clock& c, int s, GtiId r, QueueId q)
            : clock(c), isSync(s), remotePlaceId(r), queueId(q){};
    }; /*class SignalRequestInfo*/

    class PersistentSendInfo {
      public:
        GtiId remotePlaceId;
        QueueId queueId;
        bool isActive;
        PersistentSendInfo(GtiId r, QueueId q, bool active)
            : remotePlaceId(r), queueId(q), isActive(active){};
    }; /*class SignalRequestInfo*/

    class PersistentRecvInfo {
      public:
        uint64_t comm;
        bool isActive;
        PersistentRecvInfo(QueueId c, bool active) : comm(c), isActive(active){};
    }; /*class SignalRequestInfo*/

    /**
     * Constructor.
     * @param instanceName name of this module instance.
     */
    VectorClock(const char* instanceName);

    /**
     * Destructor.
     */
    virtual ~VectorClock(void);

    /**
     * @see I_VectorClock.
     */
    GTI_ANALYSIS_RETURN tick();

    /**
     * @see I_VectorClock.
     */
    GTI_ANALYSIS_RETURN init();

    //=======================================-
    // Blocking point-to-point communication
    //========================================
    GTI_ANALYSIS_RETURN receiveVClockP2P(ClockEntry* vectorClock, GtiId originId, int isSync,
                                         int isResponse, QueueId queueId);

    GTI_ANALYSIS_RETURN waitForSignal(AppId originAppRank, QueueId queueId);
    GTI_ANALYSIS_RETURN waitForResponse(AppId originAppRank, QueueId queueId);

    //=======================================-
    // Blocking collective communication
    //========================================
    GTI_ANALYSIS_RETURN allToOne(const std::vector<int>& groupRanks, QueueId queueId,
                                 AppId localRank, AppId localRoot, AppId worldRoot);
    GTI_ANALYSIS_RETURN oneToAll(AppId localRank, AppId localRoot, AppId worldRoot,
                                 const std::vector<int>& groupRanks, QueueId queueId);
    GTI_ANALYSIS_RETURN allToAll(const std::vector<int>& groupRanks, QueueId queueId,
                                 AppId localRank, AppId localRoot, AppId worldRoot);

    //=======================================-
    // Non-blocking communication
    //========================================
    GTI_ANALYSIS_RETURN handleRequest(RequestId requestHandle, AppId source, int tag);
    /* Point-to-point */
    GTI_ANALYSIS_RETURN bufferWait(QueueId queueId, RequestId requestHandle);
    GTI_ANALYSIS_RETURN bufferSignal(int appRank, QueueId queueId, RequestId requestHandle);
    GTI_ANALYSIS_RETURN signal(int isSync, AppId appRank, QueueId queueId);
    GTI_ANALYSIS_RETURN sendBufferedSignal(const SignalRequestInfo& info);
    GTI_ANALYSIS_RETURN addPersistentSendInfo(AppId appRank, QueueId queueId,
                                              RequestId requestHandle);
    GTI_ANALYSIS_RETURN addPersistentRecvInfo(RequestId requestHandle, uint64_t comm);
    /* Collective */
    GTI_ANALYSIS_RETURN bufferA2aClock(const std::vector<int>& groupRanks, QueueId queueId,
                                       RequestId request, AppId localRank, AppId localRoot,
                                       AppId worldRoot);
    GTI_ANALYSIS_RETURN bufferA2oClock(const std::vector<int>& groupRanks, QueueId queueId,
                                       RequestId request, AppId localRank, AppId localRoot,
                                       AppId worldRoot);
    GTI_ANALYSIS_RETURN bufferO2aClock(const std::vector<int>& groupRanks, QueueId queueId,
                                       RequestId request, AppId localRank, AppId localRoot,
                                       AppId worldRoot);


    //=======================================-
    // Resource-bound analysis functions
    //========================================
    GTI_ANALYSIS_RETURN bufferUnlockClock(ClockEntry* lockClock, size_t clockSize,
                                          LockId lockHandle, GtiId originId);
    GTI_ANALYSIS_RETURN unlock(LockId lockHandle, AppId appRank);
    GTI_ANALYSIS_RETURN lock(LockId lockHandle, AppId appRank);
    GTI_ANALYSIS_RETURN recvUnlockClock(ClockEntry* lockClock, size_t clockSize, LockId lockHandle,
                                        GtiId originId);
    GTI_ANALYSIS_RETURN handleLockNotify(LockId lockHandle, GtiId originId);

    /* Exposing helper functions / getters for other modules */
    GtiId getId() const { return id; };
    ClockEntry getLocalClockValue() const { return myClock.data()[id]; };
    ClockEntry getClockValue(const int appRank) const { return myClock.data()[appRank]; };
    std::string clockToStr() const { return myClock.toStr(); }

  protected:
    passVClockAcrossP2PP myPassVClockAcrossP2PFunc;
    passUnlockClockToProxyP myPassUnlockClockProxyFunc;
    passLockNotifyP myPassLockNotifyFunc;
    passUnlockClockToEndP myPassUnlockClockEndFunc;
    syncNotifyP mySyncNotifyFunc;

    I_CollStrat* myCollStratMod; /* Generic module for implementation of collective analysis functions */
    I_Place* myPlaceMod;

  private:
    uint64_t myP2pIntraLayerTime;
    uint64_t getUsecTime() const {
       struct timeval t;
       gettimeofday(&t, NULL);
       return t.tv_sec * 1000000 + t.tv_usec;
    }

    size_t myNumProcs;
    Clock myClock;
    GtiId id;

    void mergeClock(const AbstractClock& other);

    /* Point to point */
    /* For each id map the id of a p2p communication (Send+Recv) to the
     * received clock from the other process and communication information.
     * First pair entry is set by waiting process for direct merging in receive
     * function. */
    std::map<GtiId, std::unordered_map<QueueId, ClockQueue>> clockQueues;
    /* Per channel/queue we can only wait for one other process to respond in 
      * synchronous communication mode so we do not need a queue here. */
    std::map<GtiId, std::unordered_map<QueueId, Clock>> responseClocks;

    /* Non-blocking */
    std::unordered_map<RequestId, RequestType> requestTypeInfos;
    std::unordered_map<RequestId, CollRequestInfo> collInfos;
    std::unordered_map<RequestId, SignalRequestInfo> signalInfos;
    std::unordered_map<RequestId, QueueId> waitInfos;
    std::unordered_map<RequestId, PersistentSendInfo> myPersistentSendInfos;
    std::unordered_map<RequestId, PersistentRecvInfo> myPersistentRecvInfos;

    /* Resource-bound */
    std::unordered_map<LockId, std::pair<bool, ClockContext>> lockClocks;
    std::unordered_map<LockId, bool> waitingForUnlockClock;

    /* Helper functions used for blocking and non-blocking collective communication */
    GTI_ANALYSIS_RETURN internalA2a(const std::vector<int>& groupRanks, QueueId queueId,
                                    Clock& initClock, AppId localRank, AppId localRoot,
                                    AppId worldRoot);
    GTI_ANALYSIS_RETURN internalA2o(const std::vector<int>& groupRanks, QueueId queueId,
                                    Clock& clockToUse, AppId localRank, AppId localRoot,
                                    AppId worldRoot);
    GTI_ANALYSIS_RETURN internalO2a(const std::vector<int>& groupRanks, QueueId queueId,
                                    Clock& clockToUse, AppId localRank, AppId localRoot,
                                    AppId worldRoot);

}; /*class VectorClock*/

} /*namespace gti*/

#endif /*VECTORCLOCK_H */
