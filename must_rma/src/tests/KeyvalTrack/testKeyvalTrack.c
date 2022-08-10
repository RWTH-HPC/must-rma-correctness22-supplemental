/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/KeyvalTrack/layout.xml --must:analyses \
// RUN: %builddir/tests/KeyvalTrack/analysis_spec.xml \
// RUN: %must-bin-dir/testKeyvalTrack 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*MPI_TAG_UB}}
// CHECK: [MUST-REPORT]{{.*MPI_WTIME_IS_GLOBAL}}
// CHECK: [MUST-REPORT]{{.*Key created at}}
// CHECK: [MUST-REPORT]{{.*MPI_KEYVAL_INVALID}}

/**
 * @file testKeyvalTrack.c
 * A must test case for keyval tracking.
 *
 * @author Tobias Hilbrich
 *
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mustTest.h"

#define TEST_KEY(R) MPI_Initialized (&R)

/**
 * Performs the following actions:
 * (Using any number of processes)
 *
 * 1) Tests predefined keyvalues
 * 2) Creates a new keyvalue
 * 3) Frees the keyvalue
 */
int main (int argc, char** argv)
{
	int ret;
	int rank,size;

	int key,temp;

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

	//==1) Base Keyvals
	key = MPI_TAG_UB;
	TEST_KEY (key); //Test MPI_OP_NULL
	key = MPI_WTIME_IS_GLOBAL;
	TEST_KEY (key); //Test MPI_PROD

	//==2) Keyval create
	int extra_state=0;
	MPI_Keyval_create (MPI_NULL_COPY_FN, MPI_NULL_DELETE_FN, &key, &extra_state);
	temp=key;
	TEST_KEY (temp);

	//==2) Op free
	MPI_Keyval_free (&key);
	TEST_KEY (key);

	printf ("Signing off: %d of %d tasks.\n", rank, size);
	MPI_Finalize ();
	return 0;
}
