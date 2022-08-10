/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OriginChecks.h
 *       @see must::OriginChecks.
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

#include "I_OriginRMAOp.h"
#include "I_OriginChecks.h"

#include <map>
#include <vector>

#define FIBER_POOL_SIZE 10

#ifndef ORIGINCHECKS_H
#define ORIGINCHECKS_H

using namespace gti;

namespace must
{
    /**
     * Correctness checks for memory operations at the origin.
     */
    class OriginChecks : public gti::ModuleBase<OriginChecks, I_OriginChecks>
    {
        public:
            /**
            * Constructor.
            * @param instanceName name of this module instance.
            */
            OriginChecks (const char* instanceName);

            /**
             * @see I_OriginChecks::originOpStart
             */
            GTI_ANALYSIS_RETURN originOpStart(
                    MustRMAId rmaId);

            /**
             * @see I_OriginChecks::originOpComplete
             */
            GTI_ANALYSIS_RETURN originOpComplete(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustRMAId* rmaId,
                    int rmaIdLen);


            /**
             * Destructor.
             */
            virtual ~OriginChecks (void);

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
                    void* functionAddr);

            std::vector<void*> myFiberPool;
            int fiberPoolCounter;
    }; /*class OriginChecks.h*/
} /*namespace must*/

#endif /*ORIGINCHECKS.H_H*/
