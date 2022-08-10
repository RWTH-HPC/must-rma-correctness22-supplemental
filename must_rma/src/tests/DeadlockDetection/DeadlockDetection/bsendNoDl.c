/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 4 %must-bin-dir/bsendNoDl 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run-ddl %mpiexec-numproc-flag 4 %must-bin-dir/DDlbsendNoDl \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file besendNoDl.c
 * Simple send recv test for deadlock detection.
 *
 * Description:
 * There is no deadlock in this test, we call a send and a matching recv.
 * Complecation is that one of the sends is in buffered mode, which
 * needs to be ignored by BlockingState module.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
    int rank,size;
    MPI_Status status;
    void* buffer;

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
    	    MPI_Buffer_attach (malloc(1000000), 1000000);
    	    MPI_Bsend (&size, 1, MPI_INT, 1, 666, MPI_COMM_WORLD);
    		MPI_Send (&size, 1, MPI_INT, 1, 666, MPI_COMM_WORLD);
    		MPI_Buffer_detach (&buffer, &size);
    		free (buffer);
    	}

    	if (rank == 1)
    	{
    		MPI_Recv (&size, 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &status);
    		MPI_Recv (&size, 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &status);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
