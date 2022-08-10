/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MPIVectorClock.h
 *       @see must::MPIVectorClock.
 *
 *  @date 22.05.2017
 *  @author Simon Schwitanski
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"
#include "I_ArgumentAnalysis.h"
#include "I_CreateMessage.h"
#include "I_BaseConstants.h"
#include "I_DatatypeTrack.h"
#include "I_RequestTrack.h"
#include "I_LocationAnalysis.h"
#include "I_CommTrack.h"
#include "I_TSan.h"
#include "I_MPIVectorClock.h"
#include "OneSidedChecksApi.h"

#ifndef MPIVECTORCLOCK_H
#define MPIVECTORCLOCK_H

using namespace gti;

namespace must
{
    typedef std::vector<unsigned int> RMACallMatrix;
    typedef unsigned long long ClockValue;
    typedef std::vector<ClockValue> Clock;
    typedef std::map<int, void*> LastSignalMap;

    class MPIVectorClock : public gti::ModuleBase<MPIVectorClock, I_MPIVectorClock>
    {
        public:
            /**
            * Constructor.
            * @param instanceName name of this module instance.
            */
            MPIVectorClock (const char* instanceName);

            GTI_ANALYSIS_RETURN init(
                    MustParallelId pId);

            GTI_ANALYSIS_RETURN tick();

            void addPendingRMACall(int targetRank);
            GTI_ANALYSIS_RETURN receiveRMACall();

            const Clock& get();
            ClockValue* getArray();
            int length();

            ClockValue getLocalEntry();

            /**
             * Merge a clock from synchronization call.
             * (e.g. MPI_(i)Send - MPI_(i)Recv (asynchronous) or MPI_Barrier (synchronous)).
             * @param clock vector clock to merge
             */
            void merge(const Clock& clock,
                    RMACallMatrix& remoteRMACallMatrix,
                    MustParallelId pId,
                    MustLocationId lId,
                    void* ann);

            void clockReduction(MustCommType comm,
                    MustParallelId pId,
                    MustLocationId lId,
                    const Clock& sendClock,
                    RMACallMatrix& remoteRMAMatrix,
                    Clock& reducedClock,
                    void* ann);

            GTI_ANALYSIS_RETURN send(
                    MustParallelId pId,
                    MustLocationId lId,
                    int dest,
                    MustCommType comm,
                    void* ann);

            GTI_ANALYSIS_RETURN recv(
                    MustParallelId pId,
                    MustLocationId lId,
                    int source,
                    MustCommType comm,
                    void* ann);

            GTI_ANALYSIS_RETURN sendRecv(
                    MustParallelId pId,
                    MustLocationId lId,
                    int dest,
                    int source,
                    MustCommType comm,
                    void* ann);

            GTI_ANALYSIS_RETURN isend(
                    MustParallelId pId,
                    MustLocationId lId,
                    int dest,
                    MustCommType comm,
                    MustRequestType request,
                    void* ann);

            GTI_ANALYSIS_RETURN irecv(
                    MustParallelId pId,
                    MustLocationId lId,
                    int source,
                    MustCommType comm,
                    MustRequestType request);

            GTI_ANALYSIS_RETURN coll(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustCommType comm,
                    int hasRequest,
                    MustRequestType request,
                    void *ann
                    );

            GTI_ANALYSIS_RETURN completedIsend(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustRequestType request
                    );

            GTI_ANALYSIS_RETURN completedIrecv(
                    MustParallelId pId,
                    MustLocationId lId,
                    int source,
                    MustRequestType request,
                    void* ann
                    );

            GTI_ANALYSIS_RETURN completedIcoll(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustRequestType request,
                    void* ann
                    );

            void* getTSanSyncClock(int clock);
            void storeTSanSyncClock(MustParallelId pId, MustLocationId lId, void* ann);

            int translateRank (I_Comm* comm, int rank);

            GTI_ANALYSIS_RETURN happensBeforeSync(void* ann);
            GTI_ANALYSIS_RETURN happensAfterSync(void* ann);

            /**
             * Destructor.
             */
            virtual ~MPIVectorClock (void);

        protected:
            I_ParallelIdAnalysis* myPIdMod;
            I_CreateMessage* myLogger;
            I_BaseConstants* myConsts;
            I_ArgumentAnalysis* myArgMod;
            I_DatatypeTrack* myDatMod;
            I_RequestTrack* myReqMod;
            I_LocationAnalysis *myLIdMod;
            I_CommTrack* myCommMod;
            I_TSan* myTSanMod;
            I_Place* myPlaceMod;

            notifySyncP myNotifySyncFunc;

            // maps Isend requests from application to Isend requests
            // of vector clock
            std::map<MustRequestType, std::pair<MPI_Request, MPI_Request>> myIsendRequestMap;
            // stores the call info from Irecv associated with the corresponding request
            std::map<MustRequestType, MustCommType> myIrecvRequestMap;
            // stores the call info from non-blocking collective call with the corresponding request
            std::map<MustRequestType, std::pair<MustCommType, Clock>> myIcollRequestMap;

        private:
            MPI_Comm myCommWorld;
            int myRank;
            int myNumWaitCalls;
            int myRMACallMatrixSize;
            int myNProcs;
            Clock myClk;
            LastSignalMap mySignalAddrs;
            RMACallMatrix myRMACallMatrix;
            RMACallMatrix myRemoteRMACallMatrix;
            void printMPIVectorClock();

    }; /*class MPIVectorClock*/


} /*namespace must*/

#endif /*MPIVECTORCLOCK_H*/
