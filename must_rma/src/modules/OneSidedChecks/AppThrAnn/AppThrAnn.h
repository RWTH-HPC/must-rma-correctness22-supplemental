/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file AppThrAnn.h
 *       @see must::AppThrAnn.
 *
 *  @date 27.06.2017
 *  @author Simon Schwitanski
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"
#include "I_BaseConstants.h"
#include "I_WinTrack.h"
#include "I_TSan.h"
#include "I_AppThrAnn.h"
#include "OneSidedChecksApi.h"
#include "DataPool.h"
#include <set>

#ifndef APPTHRANN_H
#define APPTHRANN_H

using namespace gti;

namespace must
{
    struct AnnData;
    __thread DataPool<AnnData, 4> *myAnnPool;

    // Struct for TSan annotations.
    // Note: We do not store anything in this struct, it is rather a
    // dummy struct with overriden new and delete operators for the
    // DataPool handling. We use the object's address for the TSan
    // annotations.
    struct AnnData {
        // overload new/delete to use DataPool for memory management.
        void * operator new(size_t size){
            return myAnnPool->getData();
        }
        void operator delete(void* p, size_t){
            retData<AnnData,4>(p);
        }
    };

    class AppThrAnn : public gti::ModuleBase<AppThrAnn, I_AppThrAnn>
    {
        public:
            /**
            * Constructor.
            * @param instanceName name of this module instance.
            */
            AppThrAnn (const char* instanceName);

            /**
             * @see I_AppThrAnn::addGetOperation
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
                MustRequestType requesttype
                );

            /**
             * @see I_AppThrAnn::addPutOperation
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
                MustRequestType requesttype
                );

            /**
             * @see I_AppThrAnn::addAccumulateOperation
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
                MustRequestType requesttype
                );

            /**
             * @see I_AppThrAnn::addGetAccumulateOperation
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
                MustRequestType requesttype
                );

            /**
             * @see I_AppThrAnn:winCreate
             */
            GTI_ANALYSIS_RETURN winCreate (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win);

            /**
             * @see I_AppThrAnn:winLock
             */
            GTI_ANALYSIS_RETURN winLock (
                    MustParallelId pId,
                    MustLocationId lId,
                    int lock_type,
                    int rank,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winSync
             */
            GTI_ANALYSIS_RETURN winSync (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winUnlock
             */
            GTI_ANALYSIS_RETURN winUnlock (
                    MustParallelId pId,
                    MustLocationId lId,
                    int rank,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winUnlockAll
             */
            GTI_ANALYSIS_RETURN winUnlockAll (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winFence
             */
            GTI_ANALYSIS_RETURN winFence (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winStart
             */
            GTI_ANALYSIS_RETURN winStart (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustGroupType group,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winComplete
             */
            GTI_ANALYSIS_RETURN winComplete (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winPost
             */
            GTI_ANALYSIS_RETURN winPost (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustGroupType group,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winWait
             */
            GTI_ANALYSIS_RETURN winWait (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winFlush
             */
            GTI_ANALYSIS_RETURN winFlush (
                    MustParallelId pId,
                    MustLocationId lId,
                    int rank,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winFlushLocal
             */
            GTI_ANALYSIS_RETURN winFlushLocal (
                    MustParallelId pId,
                    MustLocationId lId,
                    int rank,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winFlushAll
             */
            GTI_ANALYSIS_RETURN winFlushAll (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:winFlushLocalAll
             */
            GTI_ANALYSIS_RETURN winFlushLocalAll (
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win);

            /**
             * @see I_AppThrAnn:send
             */
           GTI_ANALYSIS_RETURN send(
                    MustParallelId pId,
                    MustLocationId lId,
                    int dest,
                    MustCommType comm);

           /**
             * @see I_AppThrAnn:recv
             */
           GTI_ANALYSIS_RETURN recv(
                    MustParallelId pId,
                    MustLocationId lId,
                    int source,
                    MustCommType comm);

           /**
             * @see I_AppThrAnn:sendRecv
             */
           GTI_ANALYSIS_RETURN sendRecv(
                    MustParallelId pId,
                    MustLocationId lId,
                    int dest,
                    int source,
                    MustCommType comm);

           /**
             * @see I_AppThrAnn:isend
             */
           GTI_ANALYSIS_RETURN isend(
                   MustParallelId pId,
                   MustLocationId lId,
                   int dest,
                   MustCommType comm,
                   MustRequestType request);

           /**
             * @see I_AppThrAnn:irecv
             */
           GTI_ANALYSIS_RETURN irecv(
                   MustParallelId pId,
                   MustLocationId lId,
                   int source,
                   MustCommType comm,
                   MustRequestType request);

           /**
             * @see I_AppThrAnn:coll
             */
           GTI_ANALYSIS_RETURN coll(
                   MustParallelId pId,
                   MustLocationId lId,
                   MustCommType comm,
                   int hasRequest,
                   MustRequestType request
                   );

           /**
             * @see I_AppThrAnn:completedRequest
             */
           GTI_ANALYSIS_RETURN completedRequest (
                   MustParallelId pId,
                   MustLocationId lId,
                   int source,
                   MustRequestType request);

           /**
             * @see I_AppThrAnn:completedRequests
             */
           GTI_ANALYSIS_RETURN completedRequests (
                   MustParallelId pId,
                   MustLocationId lId,
                   int* sources,
                   MustRequestType *requests,
                   int count);

            GTI_ANALYSIS_RETURN annotateSignal();

            /**
             * Destructor.
             */
            virtual ~AppThrAnn (void);

        protected:
            I_ParallelIdAnalysis* myPIdMod;
            I_BaseConstants* myConsts;
            I_WinTrack* myWinMod;
            I_TSan* myTSanMod;
            propagateGetCallP myPropagateGetCall;
            propagatePutCallP myPropagatePutCall;
            propagateAccumulateCallP myPropagateAccumulateCall;
            propagateGetAccumulateCallP myPropagateGetAccumulateCall;
            propagateWinCreateP myPropagateWinCreate;
            propagateWinLockP myPropagateWinLock;
            propagateWinSyncP myPropagateWinSync;
            propagateWinUnlockP myPropagateWinUnlock;
            propagateWinUnlockAllP myPropagateWinUnlockAll;
            propagateWinFenceP myPropagateWinFence;
            propagateWinStartP myPropagateWinStart;
            propagateWinCompleteP myPropagateWinComplete;
            propagateWinPostP myPropagateWinPost;
            propagateWinWaitP myPropagateWinWait;
            propagateWinFlushP myPropagateWinFlush;
            propagateWinFlushLocalP myPropagateWinFlushLocal;
            propagateWinFlushAllP myPropagateWinFlushAll;
            propagateWinFlushLocalAllP myPropagateWinFlushLocalAll;
            propagateSendP myPropagateSend;
            propagateRecvP myPropagateRecv;
            propagateSendRecvP myPropagateSendRecv;
            propagateCollP myPropagateColl;
            propagateIsendP myPropagateIsend;
            propagateIrecvP myPropagateIrecv;
            propagateCompletedIsendP myPropagateCompletedIsend;
            propagateCompletedIrecvP myPropagateCompletedIrecv;
            propagateCompletedIcollP myPropagateCompletedIcoll;
            happensBeforeSyncP myHappensBeforeSync;
            happensAfterSyncP myHappensAfterSync;
            propagateSignalP myPropagateSignal;

            std::set<MustRequestType> myPendingSends;
            std::set<MustRequestType> myPendingRecvs;
            std::set<MustRequestType> myPendingColls;

            void happensBeforeSync();
            void happensAfterSync();
            void* genTLCAddr();
            int translateRank (I_Comm* comm, int rank);
            std::map<MustWinType, AnnData*> myRWLockMap;

    }; /*class AppThrAnn*/

} /*namespace must*/

#endif /*APPTHRANN_H*/
