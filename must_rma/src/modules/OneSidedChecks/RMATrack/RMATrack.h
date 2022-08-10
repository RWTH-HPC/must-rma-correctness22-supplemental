/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file RMATrack.h
 *       @see must::RMATrack.
 *
 *  @date 22.05.2017
 *  @author Simon Schwitanski
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"
#include "I_BaseConstants.h"
#include "I_CreateMessage.h"
#include "I_DatatypeTrack.h"
#include "I_RequestTrack.h"
#include "I_WinTrack.h"
#include "I_TSan.h"
#include "I_MPIVectorClock.h"
#include "I_LocationAnalysis.h"
#include "I_MPIVectorClock.h"
#include "I_TSanSyncClockRecorder.h"
#include "I_VectorClock.h"

#include "I_RMATrack.h"
#include "OriginRMAMap.h"
#include "TargetRMAMap.h"

#include "OneSidedChecksApi.h"

#include <map>
#include <atomic>

#ifndef RMATRACK_H
#define RMATRACK_H

using namespace gti;

namespace must
{
    // Used for tracking target op states
    enum TargetOpState {
        TARGET_OP_STATE_INIT = 0,
        TARGET_OP_STATE_CONSISTENT,
        TARGET_OP_STATE_CONSISTENT_HB,
        TARGET_OP_STATE_COUNT
    }; 

    /**
     * RMATrack for correctness checks interface implementation.
     */
    class RMATrack : public gti::ModuleBase<RMATrack, I_RMATrack>
    {
        public:
            /**
            * Constructor.
            * @param instanceName name of this module instance.
            */
            RMATrack (const char* instanceName);

            /**
             * @see I_RMATrack::init
             */
            GTI_ANALYSIS_RETURN init(MustParallelId pId);

            /**
             * @see I_RMATrack::addRemoteRMA
             */
            GTI_ANALYSIS_RETURN addRemoteRMA (
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
                void* functionAddr);

            /**
             * @see I_RMATrack::addRemoteTargetCompletion
             */
            GTI_ANALYSIS_RETURN addRemoteTargetCompletion (
                MustParallelId pId,
                MustLocationId lId,
                int origin,
                int target,
                MustRemoteIdType win,
                int epoch);

            /**
             * @see I_RMATrack::receiveSync
             */
            GTI_ANALYSIS_RETURN receiveSync (
                int remoteRank,
                int ownRank);

            /**
             * @see I_RMATrack::addGetOperation
             */
            GTI_ANALYSIS_RETURN addGetOperation (
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
                );

            /**
             * @see I_RMATrack::addPutOperation
             */
            GTI_ANALYSIS_RETURN addPutOperation (
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
                );

            /**
             * @see I_RMATrack::addAccumulateOperation
             */
            GTI_ANALYSIS_RETURN addAccumulateOperation (
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
                );

            /**
             * @see I_RMATrack::addGetAccumulateOperation
             */
            GTI_ANALYSIS_RETURN addGetAccumulateOperation (
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
                );

            /**
             * @see I_RMATrack::winCreate
             */
            GTI_ANALYSIS_RETURN winCreate(
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann);


            /**
             * @see I_RMATrack::winLock
             */
            GTI_ANALYSIS_RETURN winLock(
                MustParallelId pId,
                MustLocationId lId,
                int lock_type,
                int rank,
                MustWinType win,
                void* ann);

            /**
             * @see I_RMATrack::winUnlock
             */
            GTI_ANALYSIS_RETURN winUnlock(
                MustParallelId pId,
                MustLocationId lId,
                int rank,
                MustWinType win);

            /**
             * @see I_RMATrack::originCompletionAll
             */
            GTI_ANALYSIS_RETURN originCompletionAll (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                );

            /**
             * @see I_RMATrack::originCompletionAll
             */
            GTI_ANALYSIS_RETURN originCompletionRank (
                MustParallelId pId,
                MustLocationId lId,
                int targetRank,
                MustWinType win,
                void* ann
                );

            /**
             * @see I_RMATrack::activeTargetCompletionAll
             */
            GTI_ANALYSIS_RETURN activeTargetCompletionAll (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                );

            /**
             * @see I_RMATrack:winStart
             */
            GTI_ANALYSIS_RETURN winStart (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustGroupType group,
                    MustWinType win,
                    void* ann
                    );

            /**
             * @see I_RMATrack:winComplete
             */
            GTI_ANALYSIS_RETURN winComplete (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win,
                    void* ann
                    );

            /**
             * @see I_RMATrack:winPost
             */
            GTI_ANALYSIS_RETURN winPost (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustGroupType group,
                    MustWinType win,
                    void* ann
                    );

            /**
             * @see I_RMATrack:winWait
             */
            GTI_ANALYSIS_RETURN winWait (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win,
                    void* ann
                    );

            /**
             * @see I_RMATrack::privateWindowUpdate
             */
            GTI_ANALYSIS_RETURN privateWindowUpdate (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                );

            /**
             * @see I_RMATrack::passiveTargetCompletionAll
             */
            GTI_ANALYSIS_RETURN passiveTargetCompletionAll (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                );

            /**
             * @see I_RMATrack::passiveTargetCompletionRank
             */
            GTI_ANALYSIS_RETURN passiveTargetCompletionRank (
                MustParallelId pId,
                MustLocationId lId,
                int targetRank,
                MustWinType win,
                void* ann
                );

            /**
             * @see I_RMATrack::completedRequest
             */
            GTI_ANALYSIS_RETURN completedRequest (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustRequestType request);

            /**
             * @see I_RMATrack::completedRequests
             */
            GTI_ANALYSIS_RETURN completedRequests (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustRequestType *requests,
                    int count);

            /**
             * @see I_RMATrack::getOriginRMAOp
             */
            I_OriginRMAOp* getOriginRMAOp(MustRMAId callId);

            /**
             * @see I_RMATrack::getPersistentOriginRMAOp
             */
            I_OriginRMAOpPersistent* getPersistentOriginRMAOp(MustRMAId callId);

            /**
             * @see I_RMATrack::getTargetRMAOp
             */
            I_TargetRMAOp* getTargetRMAOp(MustRMAId callId);

            /**
             * @see I_RMATrack::getPersistentTargetRMAOp
             */
            I_TargetRMAOpPersistent* getPersistentTargetRMAOp(MustRMAId callId);

            /**
             * @see I_RMATrack::getWinLockAddr
             */
            void* getWinLockAddr(
                MustWinType win);

            /**
             * Destructor.
             */
            virtual ~RMATrack (void);

        protected:
            I_ParallelIdAnalysis* myPIdMod;
            I_CreateMessage* myLogger;
            I_BaseConstants* myConsts;
            I_DatatypeTrack* myDatMod;
            I_RequestTrack* myReqMod;
            I_GroupTrack* myGrpMod;
            I_LocationAnalysis *myLIdMod;
            I_WinTrack* myWinMod;
            I_TSan* myTSanMod;
            I_MPIVectorClock* myMPIVectorClock;
            I_TSanSyncClockRecorder* myTSanSyncClockRecorder;
            I_VectorClock* myVectorClock;
            I_Place* myPlaceMod;

        private:
            // contains all RMA accesses at the origin
            OriginRMAMap myOrigRMAMap;

            // contains all RMA accesses at the target
            TargetRMAMap myTgtRMAMap;

            std::map<MustWinType, std::map<int, std::vector<std::list<MustRMAId> > > > myTgtStateMap;
            std::set<std::pair<MustWinType, int>> myExclusiveLockSet;
            std::map<MustWinType, void*> myWinLockAddrs;

            // track window creation addresses, because it could be the begin of the concurrent region
            // if taget and origin did not synchronize no far
            std::map<MustWinType, void*> myWinCreateAddrs;

            // track access mode of window, transfer it with RMA op call information
            std::map<MustWinType, MustWinAccessMode> myWinAccessMode;

            // for general active target synchronization
            std::map<MustWinType, MustGroupType> myWinCompleteGroupMap;
            std::map<MustWinType, MustGroupType> myWinWaitGroupMap;

            // Note: Since we also have general active target synchronization, we have to use an
            // array of integers here to map calls from remote correctly.
            // Examples:
            // (1) MPI_Win_fence increases the epoch counter for all processes for a given win
            // (2) MPI_Win_start(P0,P1) increases the epoch counter for P0 and P1 for the given win.
            // (3) MPI_Win_post(P0,P1) also increases the epoch counter for P0 and P1 for the given win.
            std::map<MustWinType, std::vector<int>> myRMAActiveEpochCounter;

            // Remote active epoch counter, needed to correctly annotate general active target sync
            // We use that counter to busy-wait in MPI_Win_wait until all completion calls have arrived.
            std::map<MustWinType, std::vector<int>> myRMARemoteActiveEpochCounter;

            // store annotation addresses for *begin* of RMA epoches with *active* synchronization
            // (only in active target synchronization we have a corresponding call at the target and can
            // extract the address)
            std::map<std::pair<MustWinType, int>, void*> myRMAActiveEpochAddr;

            //Function pointers for passing target RMA op across
            passTargetRMAOpAcrossP myPassTargetRMAOpAcrossFunc;
            passTargetCompletionAcrossP myPassTargetCompletionAcrossFunc;
            notifyOriginOpStartP myNotifyOriginOpStartFunc;
            notifyOriginOpCompleteP myNotifyOriginOpCompleteFunc;
            notifyTargetOpStartP myNotifyTargetOpStartFunc;
            notifyTargetOpCompleteP myNotifyTargetOpCompleteFunc;

            // Used for unique id generation of RMA calls
            std::atomic<MustRMAId> myRMAId;

            /**
             * Generates the next unique RMA id.
             *
             * @return new unique RMA id
             */
            MustRMAId nextId();

            /**
             * Translates a rank in a given communicator to the
             * corresponding rank in MPI_COMM_WORLD.
             *
             * @param comm communicator the rank belongs to
             * @param rank rank to convert
             * @return corresponding rank in MPI_COMM_WORLD
             */
            int translateRank (I_Comm* comm, int rank);

            /**
             * Calculates a list of memory intervals that is
             * accessed at the given buffer with the given
             * datatype and count.
             *
             * @param typeinfo datatype of the access
             * @param buffer base address of the access
             * @param count number of elements accessed
             * @return accessed memory intervals
             */
            MustMemIntervalListType calcIntervalList(
                    I_Datatype * typeinfo,
                    MustAddressType buffer,
                    int count);

            /**
             * Creates and adds an origin memory operation
             * to the OriginRMAMap. The accessed memory regions
             * are calculated according to the given base address,
             * the count of elements and their MPI datatype.
             *
             * @param id RMA id of this access
             * @param pId parallel id associated with this operation
             * @param lId location id associated with this operation
             * @param isStore true iff this operation is a store operation
             * @param addr base address of this operation
             * @param count number of elements accessed
             * @param datatype datatype of accessed elements
             * @param target target rank of the associated RMA call
             * @param win window of this operation
             * @param request request id (if there is any, otherwise 0)
             * @param annAddr annotation address used for TSan annotations
             * @param functionaddr counter to function address (required to print stack trace)
             */
            void addOriginOp(
                    MustRMAId id,
                    MustParallelId pId,
                    MustLocationId lId,
                    bool isStore,
                    MustAddressType addr,
                    int count,
                    MustDatatypeType datatype,
                    int target,
                    MustWinType win,
                    MustRequestType request,
                    void* returnAddr,
                    void* annAddr,
                    void* functionaddr);

            /**
             * Passes the given target RMA operation to the given place on this tool level.
             *
             * Reasons for this to fail include the unavailability of intra layer
             * communication.
             *
             * @param origin origin rank of the associated RMA call
             * @param rmaId RMA id associated with the RMA operation
             * @param pId context of the RMA operation to pass
             * @param lId location of the RMA operation to pass
             * @param isStore true iff this operation is a store operation
             * @param isAtomic true iff this operation is atomic (accumulate operation)
             * @param isLocked true iff this operation is locked
             * @param target target rank of the associated RMA call
             * @param disp target displacement in the buffer according to window displacement unit
             * @param count number of elements accessed
             * @param originDatatype datatype of accessed elements in origin buffer
             * @param targetDatatype datatype of accessed elements in target buffer
             * @param win window of this operation
             * @param annAddr annotation address used for TSan annotations
             * @param functionaddr counter to function address (required to print stack trace)
             */
            void addTargetOp(
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
                I_Datatype* originDatatype,
                I_DatatypePersistent* targetDatatype,
                MustWinType winId,
                int epoch,
                int accessMode,
                void* returnAddr,
                void* annAddr,
                void* functionAddr);

            /**
             * Passes the given target RMA operation to the given place on this tool level.
             *
             * Reasons for this to fail include the unavailability of intra layer
             * communication.
             *
             * @param rank rank the target RMA operation should be transmitted to
             * @param origin origin rank of the associated RMA call
             * @param rmaId RMA id associated with the RMA operation
             * @param pId context of the RMA operation to pass
             * @param lId location of the RMA operation to pass
             * @param isStore true iff this operation is a store operation
             * @param isAtomic true iff this operation is atomic (accumulate operation)
             * @param target target rank of the associated RMA call
             * @param disp target displacement in the buffer according to window displacement unit
             * @param count number of elements accessed
             * @param originDatatype datatype of accessed elements in origin buffer
             * @param targetDatatype datatype of accessed elements in target buffer
             * @param win window of this operation
             * @param annAddr annotation address used for TSan annotations
             * @param toPlaceId place id of the target place on this tool level
             */
            void passTargetRMAOpAcross (
                int rank,
                int origin,
                MustRMAId rmaId,
                MustParallelId pId,
                MustLocationId lId,
                bool isStore,
                bool isAtomic,
                int target,
                int disp,
                int count,
                MustDatatypeType originDatatype,
                MustDatatypeType targetDatatype,
                MustWinType win,
                void* returnAddr,
                void* functionAddr,
                int toPlaceId);

            /**
             * Passes the given target RMA completion call to the given place on this tool level.
             *
             * Reasons for this to fail include the unavailability of intra layer
             * communication.
             *
             * @param rank rank the target RMA operation should be transmitted to
             * @param pId context of the RMA completion call to pass
             * @param lId location of the RMA completion call to pass
             * @param origin origin rank of the associated RMA call
             * @param win window of this operation
             * @param epoch RMA epoch id that is completed (only relevant for active target sync)
             */
            void passTargetCompletionAcross (
                int rank,
                MustParallelId pId,
                MustLocationId lId,
                int origin,
                MustWinType win,
                int epoch);

            /**
             * Calls a notification function that informs listeners
             * about the started origin operations.
             *
             * @param op RMA id of the started origin operation
             */
            void notifyOriginOpStart(MustRMAId op);

            /**
             * Calls a notification function that informs listeners
             * about the completed origin operations.
             *
             * @param pId parallel id of associated completion call
             * @param lId location of associated completion call
             * @param completedOps list of the origin operations that are completed
             */
            void notifyOriginOpComplete(
                    MustParallelId pId,
                    MustLocationId lId,
                    const std::list<MustRMAId>& completeOps);

            /**
             * Calls a notification function that informs listeners
             * about the started target operations.
             *
             * @param op RMA id of the started target operation
             */
            void notifyTargetOpStart(MustRMAId op);

            /**
             * Calls a notification function that informs listeners
             * about the completed target operations.
             *
             * @param pId parallel id of associated completion call
             * @param lId location of associated completion call
             * @param completedOps list of the target operations that are completed
             */
            void notifyTargetOpComplete(
                    MustParallelId pId,
                    MustLocationId lId,
                    const std::list<MustRMAId>& completeOps);

            void incrementRMAEpochRank(
                MustParallelId pId,
                MustWinType win,
                int rank
            );

            void incrementRMAEpoch (
                MustParallelId pId,
                MustWinType win
                );


    }; /*class RMATrack*/
} /*namespace must*/

#endif /*RMATRACK_H*/
