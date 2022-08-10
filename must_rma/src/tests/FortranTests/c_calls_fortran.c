/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/FortranTests/c_calls_fortranLayout.xml \
// RUN: %must-bin-dir/c_calls_fortran 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// CHECK: Hello, I am 0 of 2 tasks.
// CHECK: Signing off, rank 0

/**
 * @file cfortran.c
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a MPI_Send and calls recv_, what is a fortran subroutine that calls MPI_Recv.
 *
 *  @date 16.08.2011
 *  @author Mathias Korepkat
 */
#include <stdio.h>
#include <mpi.h>

void myrecv_ (int* out);

int main (int argc, char** argv)
{
    int rank,size,out;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

	//Enough tasks ?
	if (size < 2)
	{
		printf( "This test needs at least 2 processes!\n");
		MPI_Finalize();
		return 1;
	}

    printf ( "Hello, I am %i of %i tasks.\n", rank, size);

	if (rank == 0)
	{
		MPI_Send (&size, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
                printf ( "rank 0 send: %i\n", size);
	}

	out = -1;
	if (rank == 1)
	{
		myrecv_ (&out);
                printf ( "rank 1 received: %i\n", out);
	}

    printf ( "Signing off, rank %i\n", rank);
    MPI_Finalize ();

    return 0;
}
