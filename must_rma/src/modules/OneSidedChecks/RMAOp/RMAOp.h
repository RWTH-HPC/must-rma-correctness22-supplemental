/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file RMAOp.h
 *
 *  @date 19.06.2017
 *  @author Simon Schwitanski
 */

#ifndef RMA_OP_H
#define RMA_OP_H

#include "BaseIds.h"
#include "HandleInfoBase.h"
#include "I_RMAOp.h"

#include <map>

namespace must {
    class RMAOp: public virtual I_RMAOpPersistent, public HandleInfoBase {
        public:
            RMAOp();

            MustParallelId getPId(void);
            MustLocationId getLId(void);
            int getOrigin(void);
            int getTarget(void);
            bool isStore(void);
            MustMemIntervalListType getMemIntervals(void);
            I_Win* getWin(void);
            MustWinType getWinId(void);
            int getRMAEpoch(void);
            void* getFiber(void);
            void setFiber(void*);
            MustWinAccessMode getWinAccessMode(void);
            void* getReturnAddr(void);
            void* getFunctionAddr(void);

            virtual ~RMAOp(void);

            bool printInfo (
                std::stringstream &out,
                std::list<std::pair<MustParallelId,MustLocationId> > *pReferences); /**< @see HandleInfoBase::printInfo.*/

            std::string getResourceName (void); /**< @see HandleInfoBase::getResourceName.*/

        public:
            MustRMAId myRMAId;
            MustParallelId myPId;
            MustLocationId myLId;
            int myOrigin;
            int myTarget;
            bool myIsStore;
            MustMemIntervalListType myMemIntervals;
            I_WinPersistent* myWin;
            MustWinType myWinId;
            int myRMAEpoch;
            MustWinAccessMode myWinAccessMode;
            void* myFiber;
            void* myReturnAddr;
            void* myFunctionAddr;
    };

    typedef std::set<RMAOp*> RmaSet;
    typedef std::map<MustWinType, RmaSet> RmaMap;
    typedef std::map<MustWinType, std::set<MustRMAId>> WinRMAIdMap;
}

#endif
