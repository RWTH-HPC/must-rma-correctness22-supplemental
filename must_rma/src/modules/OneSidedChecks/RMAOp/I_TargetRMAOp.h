/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_TargetRMAOp.h
 *       @see I_TargetRMAOp.h
 *
 *  @date 20.06.2017
 *  @author Simon Schwitanski
 */

#include "MustTypes.h"
#include "BaseIds.h"

#include "I_Destructable.h"
#include "I_Request.h"
#include "I_RMAOp.h"

#ifndef I_TARGETRMAOP_H
#define I_TARGETRMAOP_H

namespace must
{
    /**
     * Interface for storage and accessing information
     * of a target RMA operation.
     */
    class I_TargetRMAOp: public virtual I_RMAOpPersistent
    {
        public:
            /*
             * Basic information
             */
            virtual bool isAtomic(void) = 0;
            virtual bool isLocked(void) = 0;
            virtual I_Datatype* getTargetDatatype(void) = 0;

            /**
             * Virtual destructor as needed
             */
            virtual ~I_TargetRMAOp(void) {}
    };/*class I_TargetRMAOp*/

    /**
     * Interface for storage and accessing information
     * of an origin RMA operation. This is the persistent
     * version of the interface. The user needs to call I_TargetRMAOpPersistent::erase
     * when he is finished with it.
     */
    class I_TargetRMAOpPersistent : public I_TargetRMAOp, public virtual I_Destructable
    {
    };/*class I_TargetRMAOpPersistent*/

}/*namespace must*/

#endif /*I_TARGETRMAOP_H*/


