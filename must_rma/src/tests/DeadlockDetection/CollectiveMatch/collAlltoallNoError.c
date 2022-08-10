/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 4 %must-bin-dir/collAlltoallNoError \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 4 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collAlltoallNoErrorlayout.xml \
// RUN: %must-bin-dir/DcollAlltoallNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file collAlltoall.c
 * A test with a correct MPI_Alltoall call (No Error).
 *
 * Description:
 * All processes execute an MPI_Alltoall with matching and valid arguments.
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
         *sbuf = NULL,
         *rbuf = NULL,
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

    sbuf = (int*) malloc (sizeof(int) * size);
    rbuf = (int*) malloc (sizeof(int) * size*2);

    MPI_Alltoall (sbuf, 1, MPI_INT, rbuf, 1, MPI_INT, MPI_COMM_WORLD);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    if (sbuf) free (sbuf);
    if (rbuf) free (rbuf);

    MPI_Finalize ();

    return 0;
}
