// RUN: %must-run --must:distributed --must:nodl --must:tsan --must:rma -np 2 %must-bin-dir/%basename_t.exe 2>&1 | FileCheck -DFILENAME=%basename_t %s

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROC_NUM 2
#define WIN_SIZE 1024

int main(int argc, char** argv) {
	int rank, size;
    MPI_Win win1, win2;
    int* win_base1;
    int* win_base2;

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != PROC_NUM) {
        printf("Wrong number of MPI processes. Expected: %d\n", PROC_NUM);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    MPI_Win_allocate(WIN_SIZE * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win_base1, &win1);
    for (int i = 0; i < WIN_SIZE; i++) {
        win_base1[i] = 0;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, win1);
        char value = 1;
        MPI_Put(&value, 1, MPI_BYTE, 1, 0, 1, MPI_BYTE, win1);
        MPI_Win_unlock(1, win1);
        
        // sync with rank 1
        MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else {
        // sync with rank 0
        int value;
        MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        win_base1[0] = 42;
    }
    
    MPI_Win_free(&win1);
    
    MPI_Finalize();
}

// CHECK-NOT: data race
