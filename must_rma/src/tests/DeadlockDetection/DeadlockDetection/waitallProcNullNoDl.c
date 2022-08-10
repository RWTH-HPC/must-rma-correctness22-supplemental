/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 %must-bin-dir/waitallProcNullNoDl \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run-ddl %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/DDlwaitallProcNullNoDl 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file waitallProcNullNoDl.c
 * Simple test with an MPI_Waitall call and MPI_PROC_NULL requests causes no deadlock (No Error).
 *
 * Description:
 * There is no deadlock in this test, we call correct and matching MPI calls.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
    MPI_Status statuses[6];
    MPI_Request requests[6];
    int buf[8];

    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    //Enough tasks ?
    	if (size < 3)
    	{
    		printf ("This test needs at least 3 processes!\n");
    		MPI_Finalize();
    		return 1;
    	}

    	//Say hello
    	printf ("Hello, I am rank %d of %d processes.\n", rank, size);

    	if (rank == 0)
    	{
    		MPI_Send (&(buf[0]), 1, MPI_INT, 1, 666, MPI_COMM_WORLD);
    		MPI_Recv (&(buf[1]), 1, MPI_INT, 1, 666, MPI_COMM_WORLD, statuses);
    	}

    	if (rank == 1)
    	{
    		MPI_Irecv (&(buf[0]), 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &(requests[0]));
    		MPI_Isend (&(buf[1]), 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &(requests[1]));
    		MPI_Irecv (&(buf[2]), 2, MPI_INT, 2, 123, MPI_COMM_WORLD, &(requests[2]));
    		MPI_Irecv (&(buf[4]), 1, MPI_INT, MPI_PROC_NULL, 666, MPI_COMM_WORLD, &(requests[3]));
    		MPI_Isend (&(buf[5]), 1, MPI_INT, MPI_PROC_NULL, 666, MPI_COMM_WORLD, &(requests[4]));
    		MPI_Irecv (&(buf[6]), 2, MPI_INT, MPI_PROC_NULL, 123, MPI_COMM_WORLD, &(requests[5]));

    		MPI_Waitall (6, requests, statuses);
    	}

    	if (rank == 2)
    	{
    	    MPI_Send (&(buf[0]), 1, MPI_INT, 1, 123, MPI_COMM_WORLD);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
