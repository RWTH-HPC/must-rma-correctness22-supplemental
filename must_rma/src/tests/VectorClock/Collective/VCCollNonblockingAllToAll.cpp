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
 * @file VCCollNonBlockingAllToAll.cpp
 *       Nonblocking all-to-all communication example
 *
 *  @date 05.06.2021
 *  @author Felix Tomski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCCollNonblockingAllToAllLayout.xml \
// RUN: %must-bin-dir/VCCollNonblockingAllToAll 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(6, 6, 3, 3)
// CHECK-DAG: shutdown(1){{.*}}clk=(6, 6, 3, 3)
// CHECK-DAG: shutdown(2){{.*}}clk=(6, 6, 4, 3)
// CHECK-DAG: shutdown(3){{.*}}clk=(6, 6, 4, 4)


#include <mpi.h>
#include <iostream>
#include <unistd.h>

#define SUBGROUP_SIZE 2

int main (int argc, char** argv)
{
	int size, rank, send_buf, recv_buf;
	MPI_Status status;
    MPI_Request request, request2;
    MPI_Group world_group, sub_group;
    MPI_Comm sub_comm;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	if (size < 4) {
		std::cerr << "This test needs at least three processes!" << std::endl;
		MPI_Finalize();
		return 0;
	}

    int groupRanks[SUBGROUP_SIZE];
    for (int i = 0; i < SUBGROUP_SIZE; i++)
        groupRanks[i] = i;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    MPI_Group_incl(world_group, SUBGROUP_SIZE, groupRanks, &sub_group);
    MPI_Comm_create(MPI_COMM_WORLD, sub_group, &sub_comm);

    if (rank < SUBGROUP_SIZE) {
        sleep(1);
        MPI_Ibarrier(sub_comm, &request);
        MPI_Barrier(sub_comm);
        MPI_Wait(&request, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
        MPI_Send(&send_buf, 1, MPI_INT, 1, 666, MPI_COMM_WORLD);
    if (rank == 1)
        MPI_Recv(&recv_buf, 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &status);

    MPI_Ibarrier(MPI_COMM_WORLD, &request);
    MPI_Ibarrier(MPI_COMM_WORLD, &request2);

    if (rank == 2)
        MPI_Send(&send_buf, 1, MPI_INT, 3, 666, MPI_COMM_WORLD);
    if (rank == 3)
        MPI_Recv(&recv_buf, 1, MPI_INT, 2, 666, MPI_COMM_WORLD, &status);

    MPI_Wait(&request, &status);
    MPI_Wait(&request2, &status);

	MPI_Finalize ();
	return 0;
}
