/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/nonBlockingNoLoss \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DP2PMatch/nonBlockingNoLosslayout.xml \
// RUN: %must-bin-dir/DnonBlockingNoLoss 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file simpleNoLoss.c
 * Simple send recv test for lost message detector.
 *
 * Description:
 * There is no lost message in this test.
 * We call multiple isends and irecvs.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size,size1[5];
    MPI_Status stats[10];
    MPI_Request reqs[10];
    int i;

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
    		for (i = 0; i < 5; i++)
    		{
    			MPI_Isend (&size, 1, MPI_INT, 1, i, MPI_COMM_WORLD, &(reqs[i]));
    		}

    		for (i = 5; i < 10; i++)
    		{
    			MPI_Irecv (&(size1[i-5]), 1, MPI_INT, 1, i, MPI_COMM_WORLD, &(reqs[i]));
    		}
    	}

    	if (rank == 1)
    	{
    		for (i = 0; i < 5; i++)
    		{
    			MPI_Irecv (&(size1[i]), 1, MPI_INT, 0, 4-i, MPI_COMM_WORLD, &(reqs[i]));
    		}

    		for (i = 5; i < 10; i++)
    		{
    			MPI_Isend (&size, 1, MPI_INT, 0, 14-i, MPI_COMM_WORLD, &(reqs[i]));
    		}
    	}

    	MPI_Waitall (10, reqs, stats);

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
