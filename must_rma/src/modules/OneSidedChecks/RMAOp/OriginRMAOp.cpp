/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OriginRMAOp.cpp
 *
 *  @date 20.06.2017
 *  @author Simon Schwitanski
 */

#include "OriginRMAOp.h"

using namespace must;

//=============================
// Constructor
//=============================
OriginRMAOp::OriginRMAOp(void):
    myRequest(NULL),
    myRequestId(0) {
        // Nothing to do
}

//=============================
// Destructor
//=============================
OriginRMAOp::~OriginRMAOp(void) {
    if (myRequest)
        myRequest->erase();
    myRequest = NULL;
}

//=============================
// getRequest
//=============================
I_Request* OriginRMAOp::getRequest(void) {
    return myRequest;
}

//=============================
// getRequestId
//=============================
MustRequestType OriginRMAOp::getRequestId(void) const {
    return myRequestId;
}
