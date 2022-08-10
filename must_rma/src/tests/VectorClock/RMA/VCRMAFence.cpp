/* This file is part of GTI (Generic Tool Infrastructure)
 *
 * Copyright (C)
 *  2008-2019 ZIH, Technische Universitaet Dresden, Federal Republic of Germany
 *  2008-2019 Lawrence Livermore National Laboratories, United States of America
 *  2013-2019 RWTH Aachen University, Federal Republic of Germany
 *
 * See the LICENSE file in the package base directory for details
 */

/**
 * @file VCRMAFence.cpp
 *       RMA fence sync example
 *
 *  @date 05.06.2021
 *  @author Felix Tomski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCRMAFenceLayout.xml \
// RUN: %must-bin-dir/VCRMAFence 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(3, 3, 3, 3)
// CHECK-DAG: shutdown(1){{.*}}clk=(3, 3, 3, 3)
// CHECK-DAG: shutdown(2){{.*}}clk=(3, 3, 3, 3)
// CHECK-DAG: shutdown(3){{.*}}clk=(3, 3, 3, 3)




#include <mpi.h>
#include <iostream>
#include <unistd.h>

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

	if (size < 2) {
		std::cerr << "This test needs at least three processes!" << std::endl;
		MPI_Finalize();
		return 0;
	}

    MPI_Info_create(&info);

    if (rank == 0) {
        int win_buf;
        MPI_Win_create(&win_buf, sizeof(int), sizeof(int), info, MPI_COMM_WORLD, &win);
        MPI_Win_fence(0, win);
        printf("Process %d target mem before fence: %d\n", rank, win_buf);
        MPI_Win_fence(0, win);
        printf("Process %d target mem after fence: %d\n", rank, win_buf);
    }
    else {
        MPI_Win_create(NULL, 0, sizeof(int), info, MPI_COMM_WORLD, &win);
        MPI_Win_fence(0, win);
        MPI_Put(&rank, 1, MPI_INT, 0, 0, 1, MPI_INT, win);
        MPI_Win_fence(0, win);
    }

    MPI_Win_free(&win);
//    std::cout << rank << " freed window" << std::endl; 

	MPI_Finalize ();
	return 0;
}
