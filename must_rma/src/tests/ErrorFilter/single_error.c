/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * This is a test for the ErrorFilter.
 *
 * Description:
 * Provokes a single MUST_ERROR_TYPEMATCH_MISMATCH.
 */

// REQUIRES: stacktrace

// ============================================================================

// RUN: %must-run --must:stacktrace backward %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/ErrorFilter_test1 | %filecheck --check-prefix=NOFILTER %s

// NOFILTER: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)

// ============================================================================

// COM: Filter with single wildcard rule
// RUN: env MUST_FILTER_FILE=%S"/Inputs/single_error/filterfile_send1.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test1 \
// RUN: | %filecheck --check-prefix=FILTER %s

// COM: Filter with single src rule
// RUN: env MUST_FILTER_FILE=%S"/Inputs/single_error/filterfile_send2.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test1 \
// RUN: | %filecheck --check-prefix=FILTER %s

// COM: Filter with single fun rule
// RUN: env MUST_FILTER_FILE=%S"/Inputs/single_error/filterfile_send3.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test1 \
// RUN: | %filecheck --check-prefix=FILTER %s

// FILTER-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// FILTER: MUST detected no MPI usage errors nor any suspicious behavior

// ============================================================================

// tests the different ways to declare an error in the filterfile with 1 error

#include <mpi.h>

int main(int argc, char **argv) {
    int myRank, numProcs;
    // MPI Initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if (numProcs != 2) {
        //       printf("This program can only be started with 2 MPI processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    int nextRank = (myRank + 1) % numProcs;
    MPI_Status status;
    int data = myRank;
    if (myRank == 0) {
        MPI_Send(&data, 1, MPI_INT, nextRank, 0, MPI_COMM_WORLD);
        MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(&data, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&data, 1, MPI_INT, nextRank, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
