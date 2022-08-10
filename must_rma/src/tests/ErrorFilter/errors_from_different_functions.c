/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * This is a test for the ErrorFilter.
 *
 * Description:
 * Provokes MUST_ERROR_TYPEMATCH_MISMATCH messages from two different functions
 * of which one is requested to be inlined and one that should not be inlined.
 */

// REQUIRES: stacktrace

// ============================================================================

// RUN: %must-run --must:stacktrace backward %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/ErrorFilter_test3 | %filecheck --check-prefix=NOFILTER %s

// NOFILTER: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_FLOAT) in the send type and at (MPI_INT) in the receive type
// NOFILTER: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_CHAR) in the send type and at (MPI_FLOAT) in the receive type

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_from_different_functions/filtertest1.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test3 \
// RUN: | %filecheck --check-prefix=FILTER1 %s

// FILTER1-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// FILTER1: MUST detected no MPI usage errors nor any suspicious behavior
// FILTER1-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_from_different_functions/filtertest2_1.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test3 \
// RUN: | %filecheck --check-prefix=FILTER2_1 %s

// FILTER2_1-NOT: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_FLOAT) in the send type and at (MPI_INT) in the receive type
// FILTER2_1: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_CHAR) in the send type and at (MPI_FLOAT) in the receive type
// FILTER2_1-NOT: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_FLOAT) in the send type and at (MPI_INT) in the receive type

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_from_different_functions/filtertest2_2.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test3 \
// RUN: | %filecheck --check-prefix=FILTER2_2 %s

// FILTER2_2-NOT: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_CHAR) in the send type and at (MPI_FLOAT) in the receive type
// FILTER2_2: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_FLOAT) in the send type and at (MPI_INT) in the receive type
// FILTER2_2-NOT: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_CHAR) in the send type and at (MPI_FLOAT) in the receive type

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_from_different_functions/filtertest2_3.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test3 \
// RUN: | %filecheck --check-prefix=FILTER2_3 %s

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_from_different_functions/filtertest2_4.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test3 \
// RUN: | %filecheck --check-prefix=FILTER2_3 %s

// FILTER2_3-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// FILTER2_3-NOT: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_FLOAT) in the send type and at (MPI_INT) in the receive type
// FILTER2_3: MUST detected no MPI usage errors nor any suspicious behavior
// FILTER2_3-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// FILTER2_3-NOT: Error: from: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_FLOAT) in the send type and at (MPI_INT) in the receive type

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_from_different_functions/filtertest.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test3 \
// RUN: | %filecheck --check-prefix=FILTER3 %s

// FILTER3-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)
// FILTER3: MUST detected no MPI usage errors nor any suspicious behavior
// FILTER3-NOT: call MPI_{{Send@0|Recv@1}}: A send and a receive operation use datatypes that do not match! Mismatch occurs at (MPI_INT) in the send type and at (MPI_FLOAT)

// ============================================================================

// tests errors in different functions

#include <mpi.h>

inline void test1(int numElements, int myRank, int numProcs, int *data) __attribute__((always_inline));
void test2(int numElements, int myRank, int numProcs, int *data) __attribute__((noinline));

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

    int data = myRank;
    const int numElements = 1;
#pragma forceinline
    test1(numElements, myRank, numProcs, &data);
    test2(numElements, myRank, numProcs, &data);

    MPI_Finalize();
    return 0;
}

void test1(int numElements, int myRank, int numProcs, int *data) {
    int nextRank = (myRank + 1) % numProcs;
    MPI_Status status;

    if (myRank == 0) {
        MPI_Send(data, numElements, MPI_FLOAT, nextRank, 0, MPI_COMM_WORLD);
        MPI_Recv(data, numElements, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(data, numElements, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Send(data, numElements, MPI_FLOAT, nextRank, 0, MPI_COMM_WORLD);
    }
}

void test2(int numElements, int myRank, int numProcs, int *data) {
    int nextRank = (myRank + 1) % numProcs;
    MPI_Status status;

    if (myRank == 0) {
        MPI_Send(data, numElements, MPI_CHAR, nextRank, 0, MPI_COMM_WORLD);
        MPI_Recv(data, numElements, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(data, numElements, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Send(data, numElements, MPI_CHAR, nextRank, 0, MPI_COMM_WORLD);
    }
}
