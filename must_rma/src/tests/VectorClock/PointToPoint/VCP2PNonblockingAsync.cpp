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
 * @file VCP2PNonblockingAsync.cpp
 *       P2P non-blocking communication 
 *
 *  @date 07.06.2021
 *  @author Felix Tomski
 */


// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCP2PNonblockingAsyncLayout.xml \
// RUN: %must-bin-dir/VCP2PNonblockingAsync 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(10, 10)
// CHECK-DAG: shutdown(1){{.*}}clk=(5, 10)

#include <mpi.h>
#include <iostream>

#define NUM_MSGS 5

int main (int argc, char** argv)
{
	int size, rank, send_buf, recv_buf;
	MPI_Status send_status[NUM_MSGS], recv_status[NUM_MSGS];
    MPI_Request send_request[NUM_MSGS], recv_request[NUM_MSGS];

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	if (size < 2) {
		std::cerr << "This test needs at least two processes!" << std::endl;
		MPI_Finalize();
		return 0;
	}

    if (rank == 0) {
        for (int i = 0; i < NUM_MSGS; i++)
            MPI_Isend(&send_buf, 1, MPI_INT, 1, 666, MPI_COMM_WORLD, &(send_request[i]));
        std::cout << "[Test] " << rank << " Passed Send 666" << std::endl;
        for (int i = 0; i < NUM_MSGS; i++)
            MPI_Irecv(&recv_buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &(recv_request[i]));
        std::cout << "[Test] " << rank << " Passed Recv 777" << std::endl;
        MPI_Waitall(NUM_MSGS, send_request, send_status);
        MPI_Waitall(NUM_MSGS, recv_request, recv_status);
//        for (int i = 0; i < NUM_MSGS; i++)
//            MPI_Wait(&(send_request[i]), &(send_status[i]));
//        for (int i = 0; i < NUM_MSGS; i++)
//            MPI_Wait(&(recv_request[i]), &(recv_status[i]));
    }
    if (rank == 1) {
        for (int i = 0; i < NUM_MSGS; i++)
            MPI_Irecv(&recv_buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &(recv_request[i]));
        std::cout << "[Test] " << rank << " Passed Recv 666" << std::endl;
        for (int i = 0; i < NUM_MSGS; i++)
            MPI_Isend(&send_buf, 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &(send_request[i]));
        std::cout << "[Test] " << rank << " Passed Send 777" << std::endl;
        MPI_Waitall(NUM_MSGS, recv_request, recv_status);
        MPI_Waitall(NUM_MSGS, send_request, send_status);
//        for (int i = 0; i < NUM_MSGS; i++)
//            MPI_Wait(&(recv_request[i]), &(recv_status[i]));
//        for (int i = 0; i < NUM_MSGS; i++)
//            MPI_Wait(&(send_request[i]), &(send_status[i]));
    }

	MPI_Finalize ();
    return 0;
}
