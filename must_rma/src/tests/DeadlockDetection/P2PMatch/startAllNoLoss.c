/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/startAllNoLoss 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DP2PMatch/startAllNoLosslayout.xml \
// RUN: %must-bin-dir/DstartAllNoLoss 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file startAllNoLoss.c
 * Simple test with MPI_Startall, no loss.
 *
 * Description:
 * There is no lost message in this test.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size, temp;
    MPI_Status statuses[2];
    MPI_Request r[2];

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
    		MPI_Send_init (&size, 1, MPI_INT, 1, 666, MPI_COMM_WORLD, &(r[0]));
    		MPI_Recv_init (&temp, 1, MPI_INT, 1, 777, MPI_COMM_WORLD, &(r[1]));
    	}

    	if (rank == 1)
    	{
    		MPI_Send_init (&size, 1, MPI_INT, 0, 777, MPI_COMM_WORLD, &(r[0]));
    		MPI_Recv_init (&temp, 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &(r[1]));
    	}

    	if (rank == 0 || rank == 1)
    	{
    		MPI_Startall (2, r);
    		MPI_Waitall (2, r, statuses);

        if(r[0] != MPI_REQUEST_NULL)
            MPI_Request_free(r);
        if(r[1] != MPI_REQUEST_NULL)
            MPI_Request_free(&(r[1]));
    	}
        
    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
