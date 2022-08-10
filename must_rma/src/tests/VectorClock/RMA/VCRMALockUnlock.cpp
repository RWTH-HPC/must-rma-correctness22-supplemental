#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <unistd.h>

/**
 * @file VCRMALockUnlock.cpp
 *       RMA lock / unlock example
 *
 *  @date 05.06.2021
 *  @author Felix Tomski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCRMALockUnlockLayout.xml \
// RUN: %must-bin-dir/VCRMALockUnlock 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(5, 7)
// CHECK-DAG: shutdown(1){{.*}}clk=(5, 7)


//int main (int argc, char** argv)
//{
//	int size, rank, send_buf, recv_buf;
//	MPI_Status status;
//    MPI_Request request;
//    MPI_Win win;
//    MPI_Info info;
//
//	MPI_Init (&argc, &argv);
//	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
//	MPI_Comm_size (MPI_COMM_WORLD, &size);
//
//	if (size < 2) {
//		std::cerr << "This test needs at least three processes!" << std::endl;
//		MPI_Finalize();
//		return 0;
//	}
//
//    MPI_Info_create(&info);
//
//    if (rank == 0) {
//        int win_buf;
//        MPI_Win_create(&win_buf, sizeof(int), sizeof(int), info, MPI_COMM_WORLD, &win);
//        MPI_Barrier(MPI_COMM_WORLD);
//    }
//    else {
//        MPI_Win_create(NULL, 0, sizeof(int), info, MPI_COMM_WORLD, &win);
//        MPI_Barrier(MPI_COMM_WORLD);
//        if (rank == 2)
//            sleep(1);
//        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
//        if (rank == 1)
//            sleep(2);
//        std::cout << rank << " locked window " << std::endl; 
//        MPI_Win_unlock(0, win);
//        std::cout << rank << " unlocked window " << std::endl; 
//    }
//
//    MPI_Win_free(&win);
//
//	MPI_Finalize ();
//	return 0;
//}

#define PROC_NUM 2
#define WIN_SIZE 1024

int main(int argc, char** argv) {
	int rank, size;
    MPI_Win win;
    int* win_base;

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < PROC_NUM) {
        printf("Wrong number of MPI processes. Expected: %d\n", PROC_NUM);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    MPI_Win_allocate(WIN_SIZE * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win_base, &win);
   for (int i = 0; i < WIN_SIZE; i++) {
       win_base[i] = 0;
   }
   MPI_Barrier(MPI_COMM_WORLD);

   if (rank == 0) {
       MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, win);
       MPI_Win_unlock(1, win);
   } else {
       MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, win);
       MPI_Win_unlock(1, win);

       MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, win);
       MPI_Win_unlock(1, win);
   }

   MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_free(&win);

    MPI_Finalize();
}


