/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/DLViewMultiLocationInMsgQueue 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.}}

/**
 * @file DLViewMultiLocationInMsgQueue.c
 * Simple deadlock resulting from tag mismatches.
 *
 * Description:
 * Wild-card receive on rank 0, each rank provides a not-matching send.
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
    		MPI_Recv (&buf, 1, MPI_INT, MPI_ANY_SOURCE, 123, MPI_COMM_WORLD, &status);
    		MPI_Wait (&request, &status);
    	}
    	else
    	{
    	    MPI_Send (&size, 1, MPI_INT, 0, 456, MPI_COMM_WORLD);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
