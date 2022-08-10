/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OriginRMAOp.h
 *
 *  @date 19.06.2017
 *  @author Simon Schwitanski
 */

#ifndef ORIGINRMAOP_H
#define ORIGINRMAOP_H

#include "I_Destructable.h"
#include "HandleInfoBase.h"
#include "I_OriginRMAOp.h"
#include "RMAOp.h"

namespace must {
    class OriginRMAOp : public RMAOp, public I_OriginRMAOpPersistent {
        public:
            OriginRMAOp(void);
            
            I_Request* getRequest(void);
            MustRequestType getRequestId(void) const;

            virtual ~OriginRMAOp(void);

        public:
            I_RequestPersistent* myRequest;
            MustRequestType myRequestId;
    };
}

#endif
