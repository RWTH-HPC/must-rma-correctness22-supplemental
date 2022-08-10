/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file waitStatusIgnore.c
 * Simple send recv where the recv is non-blocking, test for deadlock detection.
 *
 * Description:
 * There is no deadlock in this test, we call a send and a matching irecv with a wait.
 * The Wait is used with MPI_STATUS_IGNORE.
 *
 * @author Mathias Korepkat
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
    MPI_Status* status;
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
    		MPI_Send (&size, 1, MPI_INT, 1, 666, MPI_COMM_WORLD);
    	}

    	if (rank == 1)
    	{
    		MPI_Irecv (&size, 1, MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, &request);
    		MPI_Wait (&request, MPI_STATUS_IGNORE);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
