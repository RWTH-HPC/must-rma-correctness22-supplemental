/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/LeakKeyvalError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*There are 1 keys that are not freed when MPI_Finalize was issued}}

/**
 * @file LeakKeyvalError.c
 * This is a a test for the analysis LeakChecks.
 *
 * Description:
 * Leaks a keyvalue key.
 *
 *
 *  @date 19.05.2011
 *  @author Tobias Hilbrich
 */

#include <stdio.h>

#include <mpi.h>
#include "mustTest.h"

int main (int argc, char** argv)
{
	int size, rank;

	MPI_Group group;
	MPI_Comm comm;
	MPI_Datatype type;
	MPI_Errhandler err;
	int key;
	MPI_Op op;
	MPI_Request request;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	//Enough tasks ?
	if (size < 2)
	{
		printf ("This test needs at least 2 processes!\n");
		MPI_Finalize();
		return 1;
	}

	//Say hello
	printf ("Hello, I am rank %d of %d processes.\n", rank, size);

	//Do the testing
	int extra_state=0;
	MPI_Keyval_create (MPI_NULL_COPY_FN, MPI_NULL_DELETE_FN, &key, &extra_state);
	/* MISSING: MPI_Keyval_free (&key); */

	//Say bye bye
	printf ("Signing off, rank %d.\n", rank);

	MPI_Finalize ();

	return 0;
}

