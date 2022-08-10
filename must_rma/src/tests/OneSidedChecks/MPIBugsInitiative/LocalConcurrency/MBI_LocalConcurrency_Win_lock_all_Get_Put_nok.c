
// RUN: %must-run --must:distributed --must:nodl --must:tsan --must:rma -np 2 %must-bin-dir/%basename_t.exe 2>&1 | FileCheck -DFILENAME=%basename_t %s
// CHECK-DAG: data race
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
  int nprocs = -1;
  int rank = -1;
	MPI_Win win;
  int W; // Window buffer
	int NUM_ELEMT=1;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("Hello from rank %d \n", rank);

  if (nprocs < 2)
    printf("MBI ERROR: This test needs at least 2 processes to produce a bug!\n");

	MPI_Datatype type = MPI_INT;

	int target = (rank + 1) % nprocs;
  W = 4;

  MPI_Win_create(&W, NUM_ELEMT * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

  MPI_Win_lock_all(0,win);
  
	int localbuf1 = 10; 

	if (rank == 0) {
// CHECK-DAG: [[FILENAME]]:[[@LINE+1]]
  	MPI_Get(&localbuf1, NUM_ELEMT, MPI_INT, target, 0, NUM_ELEMT, type, win); /* MBIERROR1 */
// CHECK-DAG: [[FILENAME]]:[[@LINE+1]]
  	MPI_Put(&localbuf1, NUM_ELEMT, MPI_INT, target, 0, NUM_ELEMT, type, win); /* MBIERROR2 */
	}

  MPI_Win_unlock_all(win);

  MPI_Win_free(&win);

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
