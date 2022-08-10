// RUN: %must-run --must:distributed --must:nodl --must:tsan --must:rma -np 3 %must-bin-dir/%basename_t.exe 2>&1 | FileCheck -DFILENAME=%basename_t %s

#include <mpi.h>
#include <unistd.h>

#define PROC_NUM 3
#define WIN_SIZE 1024

int main (int argc, char** argv)
{
	int size, rank, send_buf, recv_buf;
	MPI_Status status;
    MPI_Request request;
    MPI_Win win;
    MPI_Info info;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	if (size != PROC_NUM) {
        printf("Wrong number of MPI processes. Expected: %d\n", PROC_NUM);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int win_buf;
    MPI_Win_create(&win_buf, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        int destrank = 2;
        MPI_Group destgroup;
        MPI_Group_incl(world_group, 1, &destrank, &destgroup);

        MPI_Win_start(destgroup, 0, win);
        int value = 42;
// CHECK-DAG: [[FILENAME]]:[[@LINE+1]]
        MPI_Put(&value, 1, MPI_INT, 2, 0, 1, MPI_INT, win);
        MPI_Win_complete(win);
    }
    else if (rank == 1) {
        int destrank = 2;
        MPI_Group destgroup;
        MPI_Group_incl(world_group, 1, &destrank, &destgroup);
        MPI_Win_start(destgroup, 0, win);
        int value;
// CHECK-DAG: [[FILENAME]]:[[@LINE+1]]
        MPI_Get(&value, 1, MPI_INT, 2, 0, 1, MPI_INT, win);
        MPI_Win_complete(win);

    } else if (rank == 2) {
        int srcrank[2] = {0,1};
        MPI_Group srcgroup;
        MPI_Group_incl(world_group, 2, &srcrank, &srcgroup);

        MPI_Win_post(srcgroup, 0, win);
        MPI_Win_wait(win);
    }

    MPI_Win_free(&win);

	MPI_Finalize ();
	return 0;
}


// CHECK-DAG: data race
