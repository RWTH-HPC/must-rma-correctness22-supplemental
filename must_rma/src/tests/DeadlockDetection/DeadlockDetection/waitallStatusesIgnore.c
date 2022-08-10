/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file waitallStatusesIgnore.c
 * Simple test with an MPI_Waitall call causes no deadlock (No Error).
 *
 * Description:
 * There is no deadlock in this test, we call correct and matching MPI calls.
 * The Wait is used with MPI_STATUSES_IGNORE.
 *
 * @author Mathias Korepkat
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
    MPI_Request requests[3];
    int buf[3];

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
    		MPI_Send (&(buf[0]), 1, MPI_INT, 1, 666, MPI_COMM_WORLD);
    		MPI_Recv (&(buf[1]), 1, MPI_INT, MPI_ANY_SOURCE, 321, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    	    MPI_Send (&(buf[2]), 1, MPI_INT, 1, 123, MPI_COMM_WORLD);
    	}

    	if (rank == 1)
    	{
    		MPI_Irecv (&(buf[0]), 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &(requests[0]));
    		MPI_Isend (&(buf[1]), 1, MPI_INT, 0, 321, MPI_COMM_WORLD, &(requests[1]));
    		MPI_Irecv (&(buf[2]), 2, MPI_INT, MPI_ANY_SOURCE, 123, MPI_COMM_WORLD, &(requests[2]));
    		MPI_Waitall (3, requests, MPI_STATUSES_IGNORE);
    	}


    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
