/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/collCallMissmatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collCallMissmatchErrorlayout.xml \
// RUN: %must-bin-dir/DcollCallMissmatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*A collective mismatch occured [(]The application executes two different collective calls on the same communicator}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*A collective mismatch occured [(]The application executes two different collective calls on the same communicator}}

/**
 * @file collCallMissmatchError.c
 * A test with a collective mismatch (Error).
 *
 * Description:
 * Process 1 issues a barrier while all other processes will not, when they issue MPI_Finalize the mismatch occurs.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
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
        MPI_Barrier (MPI_COMM_WORLD);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
