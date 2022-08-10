/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_RMATrack.h
 *       @see I_RMATrack
 *
 *  @date 13.06.2017
 *  @author Simon Schwitanski
 */

#include "GtiEnums.h"

#include "MustEnums.h"
#include "BaseIds.h"
#include "MustTypes.h"
#include "OriginRMAOp.h"
#include "TargetRMAOp.h"

#ifndef I_RMATRACK_H
#define I_RMATRACK_H


/**
 * Interface for an RMA operation tracking module. 
 *
 * Dependencies (in listed order):
 * - ParallelIdAnalysis
 * - CreateMessage
 * - ArgumentAnalysis
 * - DatatypeTrack
 * - RequestTrack
 * - WinTrack
 * - VectorClock
 */
class I_RMATrack : public gti::I_Module
{
    public:
        /**
         * Initializes the RMA id counter.
         *
         * @param pId parallel id associated with this call
         */
        virtual gti::GTI_ANALYSIS_RETURN init(
                MustParallelId pId)=0;

        /**
         * Adds a get RMA communication call.
         * @param pId parallel context
         * @param lId location id of context.
         * @param originaddr address of the buffer in which to receive the data
         * @param origincount number of entries in origin buffer
         * @param origintype datatpye of each entry in origin buffer
         * @param target rank of target
         * @param targetdisp displacement from window start of the beginning of the target buffer
         * @param targetcount number of entries in target buffer
         * @param targettype datatype of each entry in target buffer
         * @param win window object used for communication
         * @param request request object associated with call (optional, set to 0 if not used) 
         * @param ann address of dynamic annotation of AppThrAnn module
         * @param functionaddr counter to function address (required to print stack trace)
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN addGetOperation (
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
                MustRequestType requesttype,
                void* ann,
                void* functionaddr
                )=0;

        /**
         * Adds a put RMA communication call.
         * @param pId parallel context
         * @param lId location id of context.
         * @param originaddr address of the buffer of which to read the data
         * @param origincount number of entries in origin buffer
         * @param origintype datatpye of each entry in origin buffer
         * @param target rank of target
         * @param targetdisp displacement from window start of the beginning of the target buffer
         * @param targetcount number of entries in target buffer
         * @param targettype datatype of each entry in target buffer
         * @param win window object used for communication
         * @param request request object associated with call (optional, set to 0 if not used) 
         * @param ann address of dynamic annotation of AppThrAnn module
         * @param functionaddr counter to function address (required to print stack trace)
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN addPutOperation (
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
                MustRequestType requesttype,
                void* ann,
                void* functionaddr
                )=0;

        /**
         * Adds an accumulate RMA communication call.
         * @param pId parallel context
         * @param lId location id of context.
         * @param originaddr address of the buffer of which to read the data
         * @param origincount number of entries in origin buffer
         * @param origintype datatpye of each entry in origin buffer
         * @param target rank of target
         * @param targetdisp displacement from window start of the beginning of the target buffer
         * @param targetcount number of entries in target buffer
         * @param targettype datatype of each entry in target buffer
         * @param win window object used for communication
         * @param request request object associated with call (optional, set to 0 if not used) 
         * @param ann address of dynamic annotation of AppThrAnn module
         * @param functionaddr counter to function address (required to print stack trace)
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN addAccumulateOperation (
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
                MustRequestType requesttype,
                void* ann,
                void* functionaddr
                )=0;

        /**
         * Adds a get accumulate RMA communication call.
         * @param pId parallel context
         * @param lId location id of context.
         * @param originaddr address of the buffer of which to read data
         * @param origincount number of entries in origin buffer
         * @param origintype datatype of each entry in origin buffer
         * @param resultaddr address of result buffer
         * @param resultcount number of entries in result buffer
         * @param resulttype datatype of each entry in result buffer
         * @param target rank of target
         * @param targetdisp displacement from window start of the beginning of the target buffer
         * @param targetcount number of entries in target buffer
         * @param targettype datatype of each entry in target buffer
         * @param win window object used for communication
         * @param request request object associated with call (optional, set to 0 if not used) 
         * @param ann address of dynamic annotation of AppThrAnn module
         * @param functionaddr counter to function address (required to print stack trace)
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN addGetAccumulateOperation (
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
                MustRequestType requesttype,
                void* ann,
                void* functionaddr
                )=0;

        /**
         * Track window creation.
         *
         * @param pId parallel context
         * @param lId location id of context.
         * @param win window the lock is associated with
         * @param lock TSan identifier of window creation call
         */
        virtual gti::GTI_ANALYSIS_RETURN winCreate(
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann) = 0;

        /**
         * Track lock calls on windows.
         *
         * @param pId parallel context
         * @param lId location id of context.
         * @param lock_type type of lock (exclusive or shared)
         * @param rank target rank of lock
         * @param win window the lock is associated with
         * @param lock TSan identifier of the lock
         */
        virtual gti::GTI_ANALYSIS_RETURN winLock(
                MustParallelId pId,
                MustLocationId lId,
                int lock_type,
                int rank,
                MustWinType win,
                void* ann) = 0;

        /**
         * Track unlock calls on windows.
         *
         * @param pId parallel context
         * @param lId location id of context.
         * @param rank target rank of unlock
         * @param win window the lock is associated with
         */
        virtual gti::GTI_ANALYSIS_RETURN winUnlock(
                MustParallelId pId,
                MustLocationId lId,
                int rank,
                MustWinType win) = 0;

        /**
         * Returns the origin memory operation associated with the given call id.
         *
         * @param callId call id used to identify origin memory operation
         * @return origin memory operation beloging to the given call id
         */
        virtual must::I_OriginRMAOp* getOriginRMAOp(
                MustRMAId callId) = 0;

        /**
         * Persistent version of @see getOriginRMAOp .
         */
        virtual must::I_OriginRMAOpPersistent* getPersistentOriginRMAOp(
                MustRMAId callId) = 0;

        /**
         * Returns the target memory operation associated with the given call id.
         *
         * @param callId call id used to identify target memory operation
         * @return target memory operation beloging to the given call id
         */
        virtual must::I_TargetRMAOp* getTargetRMAOp(
                MustRMAId callId) = 0;

        /**
         * Persistent version of @see getTargetRMAOp .
         */
        virtual must::I_TargetRMAOpPersistent* getPersistentTargetRMAOp(
                MustRMAId callId) = 0;

        /**
         * Returns the TSan annotation address the local window locks
         * are annotated with. 
         *
         * @param win window id
         * @return TSan annotation address
         */
        virtual void* getWinLockAddr(
                MustWinType win) = 0;

        /**
	 * Receives a RMA operation from another place in the same layer.
	 */
        virtual gti::GTI_ANALYSIS_RETURN addRemoteRMA (
                int origin,
                MustRMAId rmaId,
                MustParallelId pId,
                MustLocationId lId,
                bool isStore,
                bool isAtomic,
                bool isLocked,
                int target,
                int disp,
                int count,
                MustRemoteIdType originDatatype,
                MustRemoteIdType targetDatatype,
                MustRemoteIdType win,
                int epoch,
                int accessMode,
                unsigned long long originClock,
                void* returnAddr,
                void* functionAddr) = 0;

        /**
         * Receives a remote target completion (passive target mode 
         * or general active target mode) from another place in the 
         * same layer. Synchronization through fences does not neeed
         * this analysis call, see activeTargetCompletionAll.
         * 
         * @param pId parallel context
         * @param lId location id of context.
         * @param origin origin rank of completion call
         * @param target target rank of completion call
         * @param win window identifier
         * @param epoch RMA active epoch counter (only relevant for general active target, 
         *                                        should be -1 for passive target)
         */
        virtual gti::GTI_ANALYSIS_RETURN addRemoteTargetCompletion (
                MustParallelId pId,
                MustLocationId lId,
                int origin,
                int target,
                MustRemoteIdType win,
                int epoch) = 0;

        /**
         * Receives a synchronization event with the given rank.
         *
         * @remoteRank application rank the synchronization occurred with
         * @ownRank application rank calling this notification
         */
        virtual gti::GTI_ANALYSIS_RETURN receiveSync (
                int remoteRank,
                int ownRank) = 0;


        /**
         * Called when an RMA origin completion function is called
         * which completes all outstanding origin memory operations
         * of all ranks in the corresponding window.
         *
         * @param pId parallel context
         * @param lId location id of context
         * @param win window object
         * @param ann TSan annotation
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN originCompletionAll (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                )=0;

        /**
         * Called when an RMA origin completion function is called
         * which completews all outstanding origin memory operations
         * addressed to a certain target rank in the corresponding window.
         *
         * @param pId parallel context
         * @param lId location id of context
         * @param rank rank of the target whose associated RMA calls are completed
         * @param win window object
         * @param ann TSan annotation
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN originCompletionRank (
                MustParallelId pId,
                MustLocationId lId,
                int targetRank,
                MustWinType win,
                void* ann
                )=0;
        
        /**
         * Called when MPI_Win_start is invoked (general active target sync)
         * 
         * @param pId parallel context
         * @param lId location id of context
         * @param win window object
         * @param group group of processes of start call
         * @param ann TSan annotation
         * @return gti::GTI_ANALYSIS_RETURN 
         */
        virtual gti::GTI_ANALYSIS_RETURN winStart (
                MustParallelId pId,
                MustLocationId lId,
                MustGroupType group,
                MustWinType win,
                void* ann
                )=0;


        /**
         * Called when MPI_Win_complete is invoked (general active target sync)
         * 
         * @param pId parallel context
         * @param lId location id of context
         * @param win window object
         * @param ann TSan annotation
         * @return gti::GTI_ANALYSIS_RETURN 
         */
        virtual gti::GTI_ANALYSIS_RETURN winComplete (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                )=0;

        /**
         * Called when MPI_Win_post is invoked (general active target sync)
         * 
         * @param pId parallel context
         * @param lId location id of context
         * @param win window object
         * @param group group of processes of post call
         * @param ann TSan annotation
         * @return gti::GTI_ANALYSIS_RETURN 
         */
        virtual gti::GTI_ANALYSIS_RETURN winPost (
                MustParallelId pId,
                MustLocationId lId,
                MustGroupType group,
                MustWinType win,
                void* ann
                )=0;

        /**
         * Called when MPI_Win_wait is invoked (general active target sync)
         * 
         * @param pId parallel context
         * @param lId location id of context
         * @param win window object
         * @param ann TSan annotation
         * @return gti::GTI_ANALYSIS_RETURN 
         */
        virtual gti::GTI_ANALYSIS_RETURN winWait (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                )=0;

        /**
         * Called when an RMA active target completion function is called
         * which completes all outstanding target memory operations
         * of all ranks in the corresponding window (MPI_Win_fence, MPI_Win_wait).
         *
         * Note: This function is called by the target.
         *
         * @param pId parallel context
         * @param lId location id of context
         * @param win window object
         * @param ann TSan annotation
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN activeTargetCompletionAll (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                )=0;

        /**
         * Called when an RMA private window update function is called.
         *
         * @param pId parallel context
         * @param lId location id of context
         * @param win window object
         * @param ann TSan annotation
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN privateWindowUpdate (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                )=0;

        /**
         * Called when an RMA passive target completion function is called
         * which completes all outstanding target memory operations
         * of all ranks in the corresponding window
         * (MPI_Win_flush_all, MPI_Win_unlock_all).
         *
         * Note: This function is called by the origin.
         *
         * @param pId parallel context
         * @param lId location id of context
         * @param win window object
         * @param ann TSan annotation
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN passiveTargetCompletionAll (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                )=0;

        /**
         * Called when an RMA passive target completion function is called
         * which completews all outstanding target memory operations
         * addressed to a certain target rank in the corresponding window.
         * (MPI_Win_flush, MPI_Win_unlock).
         *
         * Note: This function is called by the origin.
         *
         * @param pId parallel context
         * @param lId location id of context
         * @param rank rank of the target whose associated RMA calls are completed
         * @param win window object
         * @param ann TSan annotation
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN passiveTargetCompletionRank (
                MustParallelId pId,
                MustLocationId lId,
                int targetRank,
                MustWinType win,
                void *ann
                )=0;

        /**
         * Notification of the completion of a single request.
         *
         * @param pId parallel id of the call site.
         * @param lId location id of the call site.
         * @param request that was completed.
         * @return @see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN completedRequest (
                MustParallelId pId,
                MustLocationId lId,
                MustRequestType request) = 0;

        /**
         * Completion of an array of requests.
         *
         * @param pId parallel id of the call site.
         * @param lId location id of the call site.
         * @param requests array of completed requests
         * @param count number of requests in array.
         * @return @see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN completedRequests (
                MustParallelId pId,
                MustLocationId lId,
                MustRequestType* requests,
                int count) = 0;

}; /*class I_RMATrack*/

#endif /*I_RMATRACK_H*/
