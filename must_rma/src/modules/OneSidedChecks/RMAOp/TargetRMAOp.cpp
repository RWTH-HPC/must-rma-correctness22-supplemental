/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TargetRMAOp.cpp
 *
 *  @date 20.06.2017
 *  @author Simon Schwitanski
 */

#include "TargetRMAOp.h"

using namespace must;

//=============================
// Constructor
//=============================
TargetRMAOp::TargetRMAOp(void):
    myIsAtomic(false),
    myTargetDatatype(NULL) {
        // Nothing to do
}

//=============================
// Destructor
//=============================
TargetRMAOp::~TargetRMAOp(void) {
    if (myTargetDatatype)
        myTargetDatatype->erase();
    myTargetDatatype = NULL;
}

//=============================
// isAtomic
//=============================
bool TargetRMAOp::isAtomic(void) {
    return myIsAtomic;
}

//=============================
// isLocked
//=============================
bool TargetRMAOp::isLocked(void) {
    return myIsLocked;
}

//=============================
// getTargetDatatype
//=============================
I_Datatype* TargetRMAOp::getTargetDatatype(void) {
    return myTargetDatatype;
}
