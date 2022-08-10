/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/LeakCommError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*There are 1 communicators that are not freed when MPI_Finalize}}

/**
 * @file LeakCommError.c
 * This is a a test for the analysis LeakChecks.
 *
 * Description:
 * This test leaks a communicator.
 *
 *
 *  @date 19.05.2011
 *  @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
	int size, rank;

	MPI_Group group;
	MPI_Comm comm;

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
	MPI_Comm_group (MPI_COMM_WORLD, &group);
	MPI_Comm_create (MPI_COMM_WORLD, group, &comm);
	MPI_Group_free (&group);
	/*MISSING: MPI_Comm_free (&comm); */

	//Say bye bye
	printf ("Signing off, rank %d.\n", rank);

	MPI_Finalize ();

	return 0;
}

