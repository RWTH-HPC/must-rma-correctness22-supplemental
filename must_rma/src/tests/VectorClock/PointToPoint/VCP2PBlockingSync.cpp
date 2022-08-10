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
 * @file VCP2PBlockSync.cpp
 *       Blocking synchronous P2P example
 *
 *  @date 07.06.2021
 *  @author Felix Tomski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCP2PBlockingSyncLayout.xml \
// RUN: %must-bin-dir/VCP2PBlockingSync 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(1, 1)
// CHECK-DAG: shutdown(1){{.*}}clk=(1, 1)

#include <mpi.h>
#include <iostream>
#include <unistd.h>

#define NUM_MSGS 1

int main (int argc, char** argv)
{
	int size, rank, send_buf, recv_buf;
	MPI_Status status;
    MPI_Request request;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	if (size < 2) {
		std::cerr << "This test needs at least three processes!" << std::endl;
		MPI_Finalize();
		return 0;
	}

    if (rank == 0) {
        for (int i = 0; i < NUM_MSGS; i++)
            MPI_Ssend(&send_buf, 1, MPI_INT, 1, 666, MPI_COMM_WORLD);
//        std::cout << "[Test] " << rank << " Passed Ssend" << std::endl;
    }
    if (rank == 1) {
        for (int i = 0; i < NUM_MSGS; i++)
            MPI_Recv (&recv_buf, 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &status);
//        std::cout << "[Test] " << rank << " Passed Recv " << std::endl;
    }

	MPI_Finalize ();
	return 0;
}
