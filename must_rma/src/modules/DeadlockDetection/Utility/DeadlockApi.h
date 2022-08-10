/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DeadlockApi.h
 * 		P call definition for MUST deadlock detection API calls.
 *
 * @author Tobias Hilbrich
 * @date 15.03.2011
 */

#ifndef DEADLOCK_API_H
#define DEADLOCK_API_H

#include "I_RequestTrack.h"
#include "I_CommTrack.h"

//==Function used for propagateing blocking receive wildcard updates
inline int PpropagateRecvUpdate (
		MustParallelId pId,
		MustLocationId lId,
		int source)  {return 0;}

typedef int (*propagateRecvUpdateP) (
		MustParallelId pId,
		MustLocationId lId,
		int source);

//==Function used for propagating non-blocking receive wildcard updates
inline int PpropagateIrecvUpdate (
		MustParallelId pId,
		MustLocationId lId,
		int source,
		MustRequestType request)  {return 0;}

typedef int (*propagateIrecvUpdateP) (
		MustParallelId pId,
		MustLocationId lId,
		int source,
		MustRequestType request);

// Send part of a splitted sendrecv
inline int PsplitSend (
		MustParallelId pId,
		MustLocationId lId,
		int dest,
		int tag,
		MustCommType comm,
		MustDatatypeType type,
		int count) {return 0;}

typedef int (*splitSendP) (
		MustParallelId pId,
		MustLocationId lId,
		int dest,
		int tag,
		MustCommType comm,
        MustDatatypeType type,
        int count);

// Recv part of a splitted sendrecv
inline int PsplitRecv (
		MustParallelId pId,
		MustLocationId lId,
		int source,
		int tag,
		MustCommType comm,
        MustDatatypeType type,
        int count) {return 0;}

typedef int (*splitRecvP) (
		MustParallelId pId,
		MustLocationId lId,
		int source,
		int tag,
		MustCommType comm,
        MustDatatypeType type,
        int count);

// A start out of a splitted MPI_Startall
inline int PsplitStart (
		MustParallelId pId,
		MustLocationId lId,
		MustRequestType request) {return 0;}

typedef int (*splitStartP) (
		MustParallelId pId,
		MustLocationId lId,
		MustRequestType request);

#endif /*DEADLOCK_API_H*/
