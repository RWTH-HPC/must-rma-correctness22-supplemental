/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/collAllgathervNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collAllgathervNoErrorlayout.xml \
// RUN: %must-bin-dir/DcollAllgathervNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file collAllgathervNoError.c
 * A test with a correct MPI_Allgatherv call (No Error).
 *
 * Description:
 * All processes execute an MPI_Allgatherv with matching and valid arguments.
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
         *rcounts = NULL,
         *rdispls = NULL,
         i,
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

    temp = (int*) malloc (sizeof(int) * size*2);
    rcounts = (int*) malloc (sizeof(int) * size);
    rdispls = (int*) malloc (sizeof(int) * size);

    for (i = 0; i < size; i++)
    {
        rcounts[i] = 1;
        rdispls[i] = 2*(size - (i+1));
    }

    MPI_Allgatherv (&rank, 1, MPI_INT, temp, rcounts, rdispls, MPI_INT, MPI_COMM_WORLD);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    if (temp) free (temp);
    if (rcounts) free (rcounts);
    if (rdispls) free (rdispls);


    MPI_Finalize ();

    return 0;
}
