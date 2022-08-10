/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 4 %must-bin-dir/collScattervNoError \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 4 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collScattervNoErrorlayout.xml \
// RUN: %must-bin-dir/DcollScattervNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 4 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollScattervNoErrorlayout.xml \
// RUN: %must-bin-dir/DIntracollScattervNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file collScattervNoError.c
 * A test with a correct MPI_Scatterv call (No Error). The root sends his array in inverse order.
 *
 * Description:
 * All processes execute an MPI_Scatter call with matching and valid arguments.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
    int rank,
         size,
         *temp = NULL,
         *scounts = NULL,
         *sdispls = NULL,
         i,
         rbuf[2],
         root;
    MPI_Status status;

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

    if (rank == 1)
    {
        temp = (int*) malloc (sizeof(int) * size);
        scounts = (int*) malloc (sizeof(int) * size);
        sdispls = (int*) malloc (sizeof(int) * size);

        for (i = 0; i < size; i++)
        {
            scounts[i] = 1;
            sdispls[i] = size - (i+1);
        }
    }

    MPI_Scatterv (temp, scounts, sdispls, MPI_INT, rbuf, 1, MPI_INT, 1, MPI_COMM_WORLD);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    if (rank == 1)
    {
        if (temp) free (temp);
        if (scounts) free (scounts);
        if (sdispls) free (sdispls);
    }

    MPI_Finalize ();

    return 0;
}
