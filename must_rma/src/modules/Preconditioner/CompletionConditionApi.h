/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file CollectiveConditionApi.h
 *      P call definition for MUST preconditioned Request calls.
 *
 * @author Joachim Protze
 * @date 06.06.2011
 */

#include "MustEnums.h"
#include "MustTypes.h"

#ifndef COMPLETIONCONDITIONAPI_H
#define COMPLETIONCONDITIONAPI_H

//==Function used to forward a waitl completion that was pre-processed (which means filtered in this case)
inline int PpropagateReducedWait (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType request
        )  {return 0;}

typedef int (*propagateReducedWaitP) (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType request
        );

//==Function used to forward a waitall completion that was pre-processed
inline int PpropagateReducedWaitall (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType *requests,
        int count,
        int numProcNull /*Number of MPI_PROC_NULL associated requests that had been in the original array (they are not in this "requests" array)*/
        )  {return 0;}

typedef int (*propagateReducedWaitallP) (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType *requests,
        int count,
        int numProcNull
        );

//==Function used to forward a waitany completion that was pre-processed
inline int PpropagateReducedWaitany (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType *requests,
        int count,
        int numProcNull /*Number of MPI_PROC_NULL associated requests that had been in the original array (they are not in this "requests" array)*/
        )  {return 0;}

typedef int (*propagateReducedWaitanyP) (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType *requests,
        int count,
        int numProcNull
        );


//==Function used to forward a waitsome completion that was pre-processed
inline int PpropagateReducedWaitsome (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType *requests,
        int count,
        int numProcNull /*Number of MPI_PROC_NULL associated requests that had been in the original array (they are not in this "requests" array)*/
        )  {return 0;}

typedef int (*propagateReducedWaitsomeP) (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType *requests,
        int count,
        int numProcNull
        );


#endif /* COMPLETIONCONDITIONAPI_H */
