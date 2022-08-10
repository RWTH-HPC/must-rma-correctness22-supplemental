/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TargetRMAOp.h
 *
 *  @date 20.06.2017
 *  @author Simon Schwitanski
 */

#ifndef TARGETRMAOP_H
#define TARGETRMAOP_H

#include "I_Destructable.h"
#include "HandleInfoBase.h"
#include "I_TargetRMAOp.h"
#include "RMAOp.h"

namespace must {
    class TargetRMAOp : public RMAOp, public I_TargetRMAOpPersistent {
        public:
            TargetRMAOp(void);

            bool isAtomic(void);
            bool isLocked(void);
            I_Datatype* getTargetDatatype(void);
           
            virtual ~TargetRMAOp(void);

        public:
            bool myIsAtomic;
            bool myIsLocked; 
            I_DatatypePersistent* myTargetDatatype;
    };
}

#endif
