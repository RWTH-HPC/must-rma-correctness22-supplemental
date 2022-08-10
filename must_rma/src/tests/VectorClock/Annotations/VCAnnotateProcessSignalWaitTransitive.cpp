#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GTI_Annotations.h>

/**
 * @file VCAnnotateProcessSignalWaitTransitive.cpp
 *       Annotated process synchronization example with transitive synchronization
 *
 *  @date 17.02.2022
 *  @author Simon Schwitanski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCAnnotateProcessSignalWaitTransitiveLayout.xml \
// RUN: %must-bin-dir/VCAnnotateProcessSignalWaitTransitive 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(1, 0, 0)
// CHECK-DAG: shutdown(1){{.*}}clk=(1, 2, 0)
// CHECK-DAG: shutdown(2){{.*}}clk=(1, 2, 1)


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
        GTI_AnnotateProcessSignal(2, 0, MPI_COMM_WORLD);
    } else if (rank == 2) {
        GTI_AnnotateTick();
        GTI_AnnotateProcessWait(1, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}
