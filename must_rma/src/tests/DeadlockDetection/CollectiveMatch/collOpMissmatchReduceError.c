/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/collOpMissmatchReduceError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collOpMissmatchReduceErrorlayout.xml \
// RUN: %must-bin-dir/DcollOpMissmatchReduceError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective calls that use an operation specified conflicting operations! This rank uses the operation}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective .* that use an operation specified conflicting operations! This rank uses the operation}}

/**
 * @file collOpMissmatchReduceError.c
 * A test with an operation mismatch (Error).
 *
 * Description:
 * All processes execute an MPI_Reduce with MPI_SUM as op, except for rank 1 which uses MPI_MIN instead (ERROR).
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size, temp, root;
    MPI_Status status;
    MPI_Op op = MPI_SUM;

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

    if (rank == 1) op = MPI_MIN; /*ERROR, all ranks must use the same operation*/

    MPI_Reduce (&rank, &temp, 1, MPI_INT, op, 0, MPI_COMM_WORLD);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
