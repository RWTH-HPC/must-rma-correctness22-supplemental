/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/OpTrack/layout.xml --must:analyses \
// RUN: %builddir/tests/OpTrack/analysis_spec.xml %must-bin-dir/testOpTrack \
// RUN: 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT] Information: from: call MPI_Initialized@0: Information on operation: MPI_PROD
// CHECK: [MUST-REPORT] Information: from: call MPI_Initialized@0: Information on operation: Operation created at reference 1 References (possibly of representative): Reference 1: call MPI_Op_create@rank 0, threadid 0;
// CHECK: [MUST-REPORT] Information: from: call MPI_Initialized@0: Information on operation: MPI_OP_NULL

/**
 * @file testOpTrack.c
 * Tests operator tracking.
 *
 * @author Tobias Hilbrich
 *
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mustFeaturetested.h"

#define TEST_OP(R) fOp = MUST_Op_m2i (R); MPI_Initialized ((int*)&fOp)

void function (void *invec, void *inoutvec, int *len, MPI_Datatype *datatype)
{
	//No-op
}

/**
 * Performs the following actions:
 * (Using any number of processes)
 *
 * 1) Tests some predefined operations
 * 2) Creates and tests a user defined operation
 * 3) Frees and tests a user defined operation
 */
int main (int argc, char** argv)
{
	int ret;
	int rank,size;

	MustOpType fOp;
	MPI_Op myOp;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	printf ("Ready: %d of %d tasks.\n", rank, size);

	//Enough tasks ?
	if (size < 1)
	{
		printf ("Not enough tasks, need 1 at least.\n");
		MPI_Finalize ();
		exit (1);
	}

	//==1) Base Ops
	TEST_OP (MPI_OP_NULL); //Test MPI_OP_NULL
	TEST_OP (MPI_PROD); //Test MPI_PROD

	//==2) Op create
	MPI_Op_create (function, 1, &myOp);
	TEST_OP (myOp);

	//==2) Op free
	MPI_Op_free (&myOp);
	TEST_OP (myOp);

	printf ("Signing off: %d of %d tasks.\n", rank, size);
	MPI_Finalize ();
	return 0;
}
