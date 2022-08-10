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
 * @file I_VectorClock.h
 *       Vector clock analysis module.
 *
 *  @date 26.05.2021
 *  @author Felix Tomski
 */

#include "GtiEnums.h"
#include "I_Module.h"
#include <vector>

#ifndef I_VECTORCLOCK_H
#define I_VECTORCLOCK_H

/**
 *
 *
 * Dependencies (order as listed):
 * - X
 *
 */
class I_VectorClock : public gti::I_Module {
  public:
    /**
     * Initialize some module information.
     *
     * @return see gti::GTI_ANALYSIS_RETURN.
     */

    virtual gti::GTI_ANALYSIS_RETURN init() = 0;
    /**
     * Increment own vector clock entry by 1.
     *
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN tick() = 0;

    //=======================================-
    // Blocking point-to-point communication
    //========================================
    /**
     * Receive and buffer a P2p-signal as the receiver of the communication.
     * The buffered signal gets merged in waitForSignal() when the receiving
     * application process arrives at the receive call.
     * 
     * @param vectorClock The vector clock entries as an array of length N.
     * @param originId The ID of the sending process.
     * @param isSync 0 for asynchronous, other for synchronous communication/synchronization.
     * @param isResponse 0 for "normal" P2P-signals from a sender, other for P2P-signals from a receiver.
     * @param queueId Specifies the communication channel (e.g. MPI tags, communicator etc.)
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN receiveVClockP2P(unsigned long long* vectorClock, int originId,
                                                      int isSync, int isResponse,
                                                      uint64_t queueId) = 0;

    /**
     * Send a P2P-signal to \p appRank for communication channel \p queueId.
     *
     * @param isSync See receiveVClockP2P.
     * @param appRank The global application ID of the receiving process.
     * @param queueId See receiveVClockP2P.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN signal(int isSync, int appRank, uint64_t queueId) = 0;

    /**
     * Wait (in a blocking manner) for an appropiate P2P-signal to arrive and merge it with the own clock.
     *
     * @param originAppRank The global application ID of the process from which to receive the clock.
     * @param queueId See receiveVClockP2P.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN waitForSignal(int originAppRank, uint64_t queueId) = 0;

    /**
     * Wait (in a blocking manner) for an appropiate P2P-signal sent by the receiver of a synchronous P2P communication to arrive,
     * and merge it with the own clock.
     *
     * @param originAppRank See waitForSignal.
     * @param queueId See receiveVClockP2P.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN waitForResponse(int originAppRank, uint64_t queueId) = 0;

    //=======================================-
    // Blocking collective communication
    //========================================
    /**
     * The \p localRoot process synchronizes with each non-root process of \p groupRanks.
     *
     * @param groupRanks The list of global application IDs defined under \p queueId.
     * @param queueId The identifier for the collective group executing the collective call.
     * @param localRank The local (with in the group \p queueId) application ID of the calling process.
     * @param localRoot The local (with in the group \p queueId) application ID of the root process.
     * @param worldRoot The global application ID of the root process.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN allToOne(const std::vector<int>& groupRanks, uint64_t queueId,
                                              int localRank, int localRoot, int worldRoot) = 0;

    /* Collectives */
    /**
     * Each non-root process of \p groupRanks syncrhonizes with the \p localRoot process.
     *
     * @param localRank See allToOne.
     * @param localRoot See allToOne.
     * @param worldRoot See allToOne.
     * @param groupRanks See allToOne.
     * @param queueId See allToOne.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN oneToAll(int localRank, int localRoot, int worldRoot,
                                              const std::vector<int>& groupRanks,
                                              uint64_t queueId) = 0;

    /* Collectives */
    /**
     * All processes included in \p groupRanks synchronize with each other.
     *
     * @param groupRanks See allToOne.
     * @param queueId See allToOne.
     * @param localRank See allToOne.
     * @param localRoot The local application ID of an artifical root process used for reduce/broadcast.
     * @param worldRoot The global application ID of an artifical root process used for reduce/broadcast.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN allToAll(const std::vector<int>& groupRanks, uint64_t queueId,
                                              int localRank, int localRoot, int worldRoot) = 0;

    //=======================================-
    // Non-blocking communication
    //========================================
    /**
     * At the initialization call the whole communication channel parameters may not be yet
     * (e.g. MPI_ANY_TAG, MPI_ANY_SOURCE) on the receiver side.
     * These parameters are thus after completion of the request provide by the wrapper.
     *
     * @param requestHandle Identifier provided by the wrapper for the request.
     * @param source The application ID of the sending process. (Only for P2P)
     * @param tag Further communication channel specifier, used to build the queueId. (Only for P2P)
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN handleRequest(unsigned long requestHandle, int source,
                                                   int tag) = 0;

    /* Non-blocking point-to-point */
    /**
     * Buffer a P2P-signal under the \p requestHandle.
     * The request gets handled at handleRequest() when the app process issues the completion call.
     *
     * @param appRank See waitForVClockP2P.
     * @param queueId See waitForVClockP2P.
     * @param requestHandle See handleRequest.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN bufferSignal(int appRank, uint64_t queueId,
                                                  unsigned long requestHandle) = 0;

    /**
     * Buffer a P2P-wait under the \p requestHandle.
     * The request gets handled at handleRequest() when the app process issues the completion call.
     *
     * @param queueId See waitForVClockP2P.
     * @param requestHandle See handleRequest.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN bufferWait(uint64_t queueId, unsigned long requestHandle) = 0;

    /**
     * Register information for persistent sends.
     * The request gets handled at handleRequest() when the app process issues the completion call.
     *
     * @param appRank See waitForVClockP2P.
     * @param queueId See waitForVClockP2P.
     * @param requestHandle See handleRequest.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN addPersistentSendInfo(int appRank, uint64_t queueId,
                                                           unsigned long requestHandle) = 0;

    /**
     * Register information for persistent receives.
     * Need to build the queueId later when we get the tag.
     * Thus, this is still MPI specific and we only get the identifier for the communicator here.
     *
     * @param requestHandle See handleRequest.
     * @param comm One parameter to build the \p queueId, the other one is passed at the handleRequest call.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN addPersistentRecvInfo(unsigned long requestHandle,
                                                           uint64_t comm) = 0;

    /* Non-blocking collective */
    /**
     * Buffer information for an allToAll analysis. See allToAll.
     */
    virtual gti::GTI_ANALYSIS_RETURN bufferA2aClock(const std::vector<int>& groupRanks,
                                                    uint64_t queueId, unsigned long request,
                                                    int localRank, int localRoot,
                                                    int worldRoot) = 0;

    /**
     * Buffer information for an oneToAll analysis. See oneToAll.
     */
    virtual gti::GTI_ANALYSIS_RETURN bufferO2aClock(const std::vector<int>& groupRanks,
                                                    uint64_t queueId, unsigned long request,
                                                    int localRank, int localRoot,
                                                    int worldRoot) = 0;

    /**
     * Buffer information for an allToOne analysis. See allToOne.
     */
    virtual gti::GTI_ANALYSIS_RETURN bufferA2oClock(const std::vector<int>& groupRanks,
                                                    uint64_t queueId, unsigned long request,
                                                    int localRank, int localRoot,
                                                    int worldRoot) = 0;

    //=======================================-
    // Resource-bound analysis functions
    //========================================
    /**
     * Receive function which is triggered on the tool process that owns the resource (lock)
     * by the process that released the resource.
     * The \p lockClock is deposited under the \p lockHandle and then later forwarded
     * to another process which acquires the same resource identified by the \p lockHandle.
     *
     * @param lockClock The clock passed from the releasing process to the resource-owning process.
     * @param clockSize The size of the clock.
     * @param lockHandle Identifier for the resource. Must be identical on all (tool) processes
     *                  (responsibility of the wrapper tool).
     * @param originId The GTI ID of the releasing process.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN bufferUnlockClock(unsigned long long* lockClock,
                                                       size_t clockSize, uint64_t lockHandle,
                                                       int originId) = 0;

    /**
     * Deposits the current clock of the calling process at the tool process \p appRank owning the resource \p lockHandle.
     *
     * @param lockHandle See bufferUnlockClock.
     * @param appRank Application ID of the process which owns the resource.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN unlock(uint64_t lockHandle, int appRank) = 0;

    /**
     * Fetch the clock clock for the resource identified by \p lockHandle which lays on process \p appRank.
     *
     * @param lockHandle See bufferUnlockClock.
     * @param appRank Application ID of the process which owns the resource.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN lock(uint64_t lockHandle, int appRank) = 0;

    /**
     * Receive function which is triggered on the process that just acquired the resource \p lockHandle
     * by the process that owns the resource.
     *
     * @param lockClock The clock passed from the resource-owning process to the acquiring process
     * @param clockSize The size of the clock.
     * @param lockHandle See bufferUnlockClock.
     * @param originId The GTI ID of the releasing process.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN recvUnlockClock(unsigned long long* lockClock,
                                                     size_t clockSize, uint64_t lockHandle,
                                                     int originId) = 0;

    /**
     * Triggered on the process that owns the resource \p lockHandle.
     * Informs the triggered process that \p originId just acquired the \p lockHandle.
     * Forwards the clock for \p lockHandle (or first waits for its arrival from the releasing process)
     * to process \p originId.
     *
     * @param lockHandle See bufferUnlockClock.
     * @param originId The GTI ID of the process which sent the lock notification, i.e. the acquiring process,
              to the resource-owning process.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN handleLockNotify(uint64_t lockHandle, int originId) = 0;

    virtual int getId() const = 0;
    virtual unsigned long long getLocalClockValue() const = 0;
    virtual unsigned long long getClockValue(const int appRank) const = 0;

}; /*class I_VectorClock*/

#endif /*I_VECTORCLOCK_H */
