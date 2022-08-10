#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GTI_Annotations.h>

/**
 * @file VCAnnotateProcessSignalWait.cpp
 *       Annotated process synchronization example
 *
 *  @date 17.02.2022
 *  @author Simon Schwitanski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCAnnotateProcessSignalWaitLayout.xml \
// RUN: %must-bin-dir/VCAnnotateProcessSignalWait 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(1, 0, 0)
// CHECK-DAG: shutdown(1){{.*}}clk=(1, 2, 0)
// CHECK-DAG: shutdown(2){{.*}}clk=(0, 0, 0)


int main(int argc, char** argv) {
	int rank, size;
    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        GTI_AnnotateTick();
        GTI_AnnotateProcessSignal(1, 0, MPI_COMM_WORLD);

    } else if (rank == 1) {
        GTI_AnnotateTick();
        GTI_AnnotateTick();
        GTI_AnnotateProcessWait(0, 0, MPI_COMM_WORLD);
    } else if (rank == 2) {
    }

    MPI_Finalize();
}
