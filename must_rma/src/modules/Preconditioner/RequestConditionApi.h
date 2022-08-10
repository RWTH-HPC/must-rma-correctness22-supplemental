/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file RequestConditionApi.h
 *      P call definition for MUST preconditioned Request calls.
 *
 * @author Joachim Protze
 * @date 06.06.2011
 */

#include "I_RequestTrack.h"

#ifndef REQUESTCONDITIONAPI_H
#define REQUESTCONDITIONAPI_H

//==Function used for propagating non-blocking receive wildcard updates
inline int PpropagateRequestRealComplete (
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustRequestType request)  {return 0;}

typedef int (*propagateRequestRealCompleteP) (
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustRequestType request);

//==Function used for propagating non-blocking receive wildcard updates
inline int PpropagateRequestsRealComplete (
        MustParallelId pId,
        MustLocationId lId,
        int *sources,
        MustRequestType* requests,
        int count)  {return 0;}

typedef int (*propagateRequestsRealCompleteP) (
        MustParallelId pId,
        MustLocationId lId,
        int *sources,
        MustRequestType* requests,
        int count);

#endif /* REQUESTCONDITIONAPI_H */
