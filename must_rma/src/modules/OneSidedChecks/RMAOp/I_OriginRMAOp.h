/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_OriginRMAOp.h
 *       @see I_OriginRMAOp.h
 *
 *  @date 20.06.2017
 *  @author Simon Schwitanski
 */

#include "MustTypes.h"
#include "BaseIds.h"

#include "I_Destructable.h"
#include "I_Request.h"
#include "I_RMAOp.h"

#ifndef I_ORIGINRMAOP_H
#define I_ORIGINRMAOP_H

namespace must
{
    /**
     * Interface for storage and accessing information
     * of an origin RMA operation.
     */
    class I_OriginRMAOp: public virtual I_RMAOpPersistent
    {
        public:
            /*
             * Basic information
             */
            virtual I_Request* getRequest(void) = 0;
            virtual MustRequestType getRequestId(void) const = 0;

            /**
             * Virtual destructor as needed
             */
            virtual ~I_OriginRMAOp(void) {}
    };/*class I_OriginRMAOp*/

    /**
     * Interface for storage and accessing information
     * of an origin RMA operation. This is the persistent
     * version of the interface. The user needs to call I_OriginRMAOpPersistent::erase
     * when he is finished with it.
     */
    class I_OriginRMAOpPersistent : public I_OriginRMAOp, public virtual I_Destructable
    {
    };/*class I_OriginRMAOpPersistent*/

}/*namespace must*/

#endif /*I_ORIGINRMAOP_H*/


