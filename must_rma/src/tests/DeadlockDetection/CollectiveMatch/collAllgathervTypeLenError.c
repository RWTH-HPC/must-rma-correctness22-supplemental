/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 4 \
// RUN: %must-bin-dir/collAllgathervTypeLenError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 4 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collAllgathervTypeLenErrorlayout.xml \
// RUN: %must-bin-dir/DcollAllgathervTypeLenError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective calls use [(]datatype,count[)] pairs that span type signatures of different length!.*The first element of .* that did not fit into the .* operation is at [(]MPI_INT[)]*}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length!}}

/**
 * @file collAllgathervTypeLenError.c
 * A test with a type matching error in MPI_Allgatherv call (ERROR).
 *
 * Description:
 * All processes execute an MPI_Allgatherv where a type match error happens.
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
        printf ("This test needs at least 4 processes!\n");
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

    if (rank == 3)
        rcounts[1] = 2; //THIS IS THE ERROR must be 1

    MPI_Allgatherv (&rank, 1, MPI_INT, temp, rcounts, rdispls, MPI_INT, MPI_COMM_WORLD);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    if (temp) free (temp);
    if (rcounts) free (rcounts);
    if (rdispls) free (rdispls);


    MPI_Finalize ();

    return 0;
}
