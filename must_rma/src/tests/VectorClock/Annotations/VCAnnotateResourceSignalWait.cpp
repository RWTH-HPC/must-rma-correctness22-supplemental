#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GTI_Annotations.h>

/**
 * @file VCAnnotateResourceSignalWait.cpp
 *       Annotated process synchronization example with transitive synchronization
 *
 *  @date 17.02.2022
 *  @author Simon Schwitanski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCAnnotateResourceSignalWaitLayout.xml \
// RUN: %must-bin-dir/VCAnnotateResourceSignalWait 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(2, 1, 2)
// CHECK-DAG: shutdown(1){{.*}}clk=(1, 2, 1)
// CHECK-DAG: shutdown(2){{.*}}clk=(1, 1, 2)

#define WIN_SIZE 1024

int main(int argc, char** argv) {
	int rank, size;
    MPI_Win win;
    int* win_base;

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Win_allocate(WIN_SIZE * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win_base, &win);
    for (int i = 0; i < WIN_SIZE; i++) {
        win_base[i] = 0;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        GTI_AnnotateTick();
        sleep(1);
        GTI_AnnotateLock(win, 0, MPI_LOCK_EXCLUSIVE);

    } else if (rank == 1) {
        GTI_AnnotateTick();
        
    } else if (rank == 2) {
        GTI_AnnotateTick();
        GTI_AnnotateUnlock(win, 0);
    }

//    MPI_Win_free(&win);
    MPI_Finalize();
}
