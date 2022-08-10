/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * This is a test for the ErrorFilter.
 *
 * Description:
 * Provokes a MUST_ERROR_TYPEMATCH_MISMATCH and a MUST_WARNING_INTEGER_ZERO inside the same
 * function.
 */

// REQUIRES: stacktrace

// ============================================================================

// RUN: %must-run --must:stacktrace backward %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/ErrorFilter_test2 | %filecheck --check-prefix=NOFILTER %s

// NOFILTER: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// NOFILTER: Warning: from: call MPI_Send@1: Argument 2 (count) is zero, which is correct but unusual! References (possibly of representative)

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids/filterfile1_1.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test2 \
// RUN: | %filecheck --check-prefix=FILTER1 %s

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids/filterfile1_2.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test2 \
// RUN: | %filecheck --check-prefix=FILTER1 %s

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids/filterfile1_3.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test2 \
// RUN: | %filecheck --check-prefix=FILTER1 %s

// FILTER1-NOT: Warning: from: call MPI_Send@1: Argument 2 (count) is zero, which is correct but unusual! References (possibly of representative)
// FILTER1: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT) in the receive type
// FILTER1-NOT: Warning: from: call MPI_Send@1: Argument 2 (count) is zero, which is correct but unusual! References (possibly of representative)

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids/filterfile2_1.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test2 \
// RUN: | %filecheck --check-prefix=FILTER2 %s

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids/filterfile2_2.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test2 \
// RUN: | %filecheck --check-prefix=FILTER2 %s

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids/filterfile2_3.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test2 \
// RUN: | %filecheck --check-prefix=FILTER2 %s

// FILTER2-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// FILTER2: Warning: from: call MPI_Send@1: Argument 2 (count) is zero, which is correct but unusual! References (possibly of representative)
// FILTER2-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids/filterfile3.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test2 \
// RUN: | %filecheck --check-prefix=FILTER3 %s

// FILTER3-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// FILTER3-NOT: Warning: from: call MPI_Send@1: Argument 2 (count) is zero, which is correct but unusual! References (possibly of representative)
// FILTER3: MUST detected no MPI usage errors nor any suspicious behavior
// FILTER3-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// FILTER3-NOT: Warning: from: call MPI_Send@1: Argument 2 (count) is zero, which is correct but unusual! References (possibly of representative)

// ============================================================================

// 2 errors tests different combinations

#include <mpi.h>

int main(int argc, char **argv) {
    int myRank, numProcs;
    // MPI Initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if (numProcs != 2) {
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

    if (myRank == 0) {
        MPI_Send(&data, 1, MPI_INT, nextRank, 0, MPI_COMM_WORLD);
        MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&data, 0, MPI_INT, nextRank, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
