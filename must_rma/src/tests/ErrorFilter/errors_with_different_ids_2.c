/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * This is a test for the ErrorFilter.
 *
 * Description:
 * Provokes a MUST_ERROR_MESSAGE_LOST and a MUST_ERROR_LEAK_REQUEST inside a single function.
 */

// REQUIRES: stacktrace

// ============================================================================

// RUN: %must-run --must:stacktrace backward %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/ErrorFilter_test4 | %filecheck --check-prefix=NOFILTER %s

// NOFILTER: Error: from: call MPI_Isend@0: The application fails to match a point-to-point operation before it issues MPI_Finalize. The outstanding send point-to-point message of rank 0 needs to be matched by a message to rank 1
// NOFILTER: Error: from: call MPI_Isend@0: There are 255 requests that are not freed when MPI_Finalize was issued, a quality application should free all MPI resources before calling MPI_Finalize.

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids_2/testfilter1.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test4 \
// RUN: | %filecheck --check-prefix=FILTER1 %s

// FILTER1-NOT: Error: from: call MPI_Isend@0: The application fails to match a point-to-point operation before it issues MPI_Finalize. The outstanding send point-to-point message of rank 0 needs to be matched by a message to rank 1
// FILTER1-NOT: Error: from: call MPI_Isend@0: There are 255 requests that are not freed when MPI_Finalize was issued, a quality application should free all MPI resources before calling MPI_Finalize.
// FILTER1: MUST detected no MPI usage errors nor any suspicious behavior
// FILTER1-NOT: Error: from: call MPI_Isend@0: The application fails to match a point-to-point operation before it issues MPI_Finalize. The outstanding send point-to-point message of rank 0 needs to be matched by a message to rank 1
// FILTER1-NOT: Error: from: call MPI_Isend@0: There are 255 requests that are not freed when MPI_Finalize was issued, a quality application should free all MPI resources before calling MPI_Finalize.

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids_2/testfilter2.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test4 \
// RUN: | %filecheck --check-prefix=FILTER2_1 %s

// FILTER2_1: Error: from: call MPI_Isend@0: There are 255 requests that are not freed when MPI_Finalize was issued, a quality application should free all MPI resources before calling MPI_Finalize.

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids_2/testfilter3.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test4 \
// RUN: | %filecheck --check-prefix=FILTER2_2 %s

// FILTER2_2-NOT: Error: from: call MPI_Isend@0: There are 255 requests that are not freed when MPI_Finalize was issued, a quality application should free all MPI resources before calling MPI_Finalize.
// FILTER2_2: Error: from: call MPI_Isend@0: The application fails to match a point-to-point operation before it issues MPI_Finalize. The outstanding send point-to-point message of rank 0 needs
// FILTER2_2-NOT: Error: from: call MPI_Isend@0: There are 255 requests that are not freed when MPI_Finalize was issued, a quality application should free all MPI resources before calling MPI_Finalize.

// ============================================================================

// RUN: env MUST_FILTER_FILE=%S"/Inputs/errors_with_different_ids_2/testfilter4.txt" %must-run \
// RUN: --must:stacktrace backward %mpiexec-numproc-flag 2 %must-bin-dir/ErrorFilter_test4 \
// RUN: | %filecheck --check-prefix=FILTER3 %s

// FILTER3-NOT: Error: from: call MPI_Isend@0: The application fails to match a point-to-point operation before it issues MPI_Finalize. The outstanding send point-to-point message of rank 0 needs
// FILTER3-NOT: Error: from: call MPI_Isend@0: There are 255 requests that are not freed when MPI_Finalize was issued, a quality application should free all MPI resources before calling MPI_Finalize.
// FILTER3: MUST detected no MPI usage errors nor any suspicious behavior
// FILTER3-NOT: Error: from: call MPI_Isend@0: The application fails to match a point-to-point operation before it issues MPI_Finalize. The outstanding send point-to-point message of rank 0 needs
// FILTER3-NOT: Error: from: call MPI_Isend@0: There are 255 requests that are not freed when MPI_Finalize was issued, a quality application should free all MPI resources before calling MPI_Finalize.

// ============================================================================

// similar to test2 test with 2 errors

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

    int data = myRank;
    MPI_Request request;

    const unsigned request_count = 255;
    for (int i = 0; i < request_count; ++i) {
        MPI_Status status;

        if (myRank == 0) {
            MPI_Isend(&data, 1, MPI_FLOAT, nextRank, 1, MPI_COMM_WORLD, &request);
        }
    }

    MPI_Finalize();
    return 0;
}
