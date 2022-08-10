// RUN: %must-run --must:distributed --must:nodl --must:tsan --must:rma -np 2 %must-bin-dir/%basename_t.exe 2>&1 | FileCheck -DFILENAME=%basename_t %s

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROC_NUM 2
#define WIN_SIZE 1024

int main(int argc, char** argv) {
	int rank, size;
    MPI_Win win;
    int* win_base;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != PROC_NUM) {
        printf("Wrong number of MPI processes. Expected: %d\n", PROC_NUM);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    MPI_Win_allocate(WIN_SIZE * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win_base, &win);
    for (int i = 0; i < WIN_SIZE; i++) {
        win_base[i] = 0;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /* conflicting put and get to / from rank 1 */
    if (rank == 0) {
        int value = 1337;
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, win);
        /* conflicting put operations */
// CHECK-DAG: [[FILENAME]]:[[@LINE+1]]
        MPI_Put(&value, 1, MPI_LONG_LONG, 1, 0, 1, MPI_LONG_LONG, win);
        //MPI_Barrier(MPI_COMM_WORLD);
// CHECK-DAG: [[FILENAME]]:[[@LINE+1]]
        MPI_Put(&value, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
        MPI_Win_unlock(1, win);
    } else {
        //MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Win_free(&win);

    MPI_Finalize();
}

// CHECK-DAG: data race