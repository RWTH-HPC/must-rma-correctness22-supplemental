/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TargetChecks.h
 *       @see must::TargetChecks.
 *
 *  @date 13.06.2017
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
#include "I_LocationAnalysis.h"
#include "I_RMATrack.h"

#include "I_TargetRMAOp.h"
#include "I_TargetChecks.h"

#include <map>


#ifndef TARGETCHECKS_H
#define TARGETCHECKS_H

using namespace gti;

namespace must
{
    /**
     * Correctness checks for memory operations at the target.
     */
    class TargetChecks : public gti::ModuleBase<TargetChecks, I_TargetChecks>
    {
        public:
            /**
            * Constructor.
            * @param instanceName name of this module instance.
            */
            TargetChecks (const char* instanceName);

            /**
             * @see I_TargetChecks::targetOpStart
             */
            GTI_ANALYSIS_RETURN targetOpStart(
                    MustRMAId rmaId);

            /**
             * @see I_TargetChecks::targetOpComplete
             */
            GTI_ANALYSIS_RETURN targetOpComplete(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustRMAId* rmaId,
                    int rmaIdLen);


            /**
             * Destructor.
             */
            virtual ~TargetChecks (void);

        protected:
            I_ParallelIdAnalysis* myPIdMod;
            I_CreateMessage* myLogger;
            I_BaseConstants* myConsts;
            I_DatatypeTrack* myDatMod;
            I_RequestTrack* myReqMod;
            I_LocationAnalysis *myLIdMod;
            I_WinTrack* myWinMod;
            I_TSan* myTSanMod;
            I_RMATrack* myRMAMod;

        private:
            void annotateMemAccess(
                    MustParallelId pId,
                    MustLocationId lId,
                    const MustMemIntervalListType& memIntervals,
                    bool isStore,
                    void* returnAddr,
                    void* funcAddr);

            void annotateAtomicAccesses(
                    MustParallelId pId,
                    MustLocationId lId,
                    bool isStore,
                    const MustMemIntervalListType& memIntervals,
                    MustMpiDatatypePredefined baseType);

            MustMpiDatatypePredefined extractBasetype(I_Datatype* datatype);

            std::map<int, void*> myFiberPool;

    }; /*class TargetChecks.h*/
} /*namespace must*/

#endif /*TARGETCHECKS.H_H*/
