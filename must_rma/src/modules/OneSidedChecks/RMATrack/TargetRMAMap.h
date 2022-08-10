/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TargetRMAMap.h
 *
 *  @date 22.06.2017
 *  @author Simon Schwitanski
 */

#ifndef TARGET_RMA_MAP_H
#define TARGET_RMA_MAP_H

#include <map>

#include "RMAMap.h"
#include "TargetRMAOp.h"
#include "MustTypes.h"

namespace must {

/**
 * Implements a map that stores TargetRMAOp objects.
 */
class TargetRMAMap: public RMAMap<TargetRMAOp> {
};

}

#endif
