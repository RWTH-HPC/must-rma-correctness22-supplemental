/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_RMAOp.h
 *       @see I_RMAOp.h
 *
 *  @date 20.06.2017
 *  @author Simon Schwitanski
 */

#include "MustTypes.h"
#include "BaseIds.h"

#include "I_Destructable.h"
#include "I_Win.h"
#include "StridedBlock.h"

#ifndef I_RMAOP_H
#define I_RMAOP_H

namespace must
{
    // Tracking window access mode for RMA op (passive or active)
    enum MustWinAccessMode {
        WINDOW_ACCESS_PASSIVE = 0,
        WINDOW_ACCESS_ACTIVE = 1
    };


    /**
     * Interface for storage and accessing information
     * of an RMA operation.
     */
    class I_RMAOp
    {
        public:
            /*
             * Basic information
             */
            virtual MustParallelId getPId(void) = 0;
            virtual MustLocationId getLId(void) = 0;
            virtual int getOrigin(void) = 0;
            virtual int getTarget(void) = 0;
            virtual bool isStore(void) = 0;
            virtual MustMemIntervalListType getMemIntervals(void) = 0;
            virtual I_Win* getWin(void) = 0;
            virtual MustWinType getWinId(void) = 0;
            virtual int getRMAEpoch(void) = 0;
            virtual MustWinAccessMode getWinAccessMode(void) = 0;
            virtual void* getFiber(void) = 0;
            virtual void setFiber(void*) = 0;
            virtual void* getReturnAddr(void) = 0;
            virtual void* getFunctionAddr(void) = 0;

            /**
             * Virtual destructor as needed
             */
            virtual ~I_RMAOp(void) {}
    };/*class I_RMAOp*/

    /**
     * Interface for storage and accessing Information
     * of an RMA operation. This is the persistent
     * version of the interface. The user needs to call I_RMAOpPersistent::erase
     * when he is finished with it.
     */
    class I_RMAOpPersistent : public I_RMAOp, public virtual I_Destructable
    {
    };/*class I_RMAOpPersistent*/

}/*namespace must*/

#endif /*I_RMAOP_H*/


