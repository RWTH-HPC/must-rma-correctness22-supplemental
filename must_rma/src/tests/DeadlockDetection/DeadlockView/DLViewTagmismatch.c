/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/DLViewTagmismatch \
// RUN: 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.}}

/**
 * @file DLViewTagmismatch.c
 * Simple deadlock resulting from a tag mismatch.
 *
 * Description:
 * Tag mismatch between send and receive operations causes a deadlock.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size, buf;
    MPI_Status status;
    MPI_Request request;

    MPI_Init(&argc,&argv);
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

    	if (rank == 0)
    	{
    		MPI_Isend (&size, 1, MPI_INT, 1, 200, MPI_COMM_WORLD, &request);
    		MPI_Recv (&buf, 1, MPI_INT, 1, 200, MPI_COMM_WORLD, &status);
    		MPI_Wait (&request, &status);
    	}

    	if (rank == 1)
    	{
    	    MPI_Isend (&size, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &request);
    	    MPI_Recv (&buf, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
    	    MPI_Wait (&request, &status);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
