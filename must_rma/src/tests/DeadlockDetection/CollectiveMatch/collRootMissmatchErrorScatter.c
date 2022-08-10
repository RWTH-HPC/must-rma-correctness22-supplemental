/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/collRootMissmatchErrorScatter 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collRootMissmatchErrorScatterlayout.xml \
// RUN: %must-bin-dir/DcollRootMissmatchErrorScatter 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective operations that use a root process specified conflicting roots! This collective uses rank .* as root .*The conflicting operation uses rank .* as root}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective operations that use a root process specified conflicting roots! This collective uses rank .* as root .*The conflicting operation uses rank .* as root}}

/**
 * @file collRootMissmatchErrorScatter.c
 * A test with root mismatch (Error).
 *
 * Description:
 * All processes execute a scatter with a root of 0, except for rank 1 which uses 1 as its root (ERROR).
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
    int rank,size, temp, root, *tempArray;
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

    root = 0;
    if (rank == 1) root = 1; /*ERROR, rank 1 uses root of 1 instead of 0*/

    tempArray = (int*) malloc (sizeof(int)* size);

    MPI_Scatter (&tempArray, 1, MPI_INT, &temp, 1, MPI_INT, root, MPI_COMM_WORLD);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}

/*EOF*/
