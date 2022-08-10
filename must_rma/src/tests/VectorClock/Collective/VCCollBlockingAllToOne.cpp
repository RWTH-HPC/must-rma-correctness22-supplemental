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
 * @file VCCollBlockingAllToOne.cpp
 *       Blocking all-to-one communication example
 *
 *  @date 05.06.2021
 *  @author Felix Tomski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCCollBlockingAllToOneLayout.xml \
// RUN: %must-bin-dir/VCCollBlockingAllToOne 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2)
// CHECK-DAG: shutdown(1){{.*}}clk=(0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
// CHECK-DAG: shutdown(5){{.*}}clk=(0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0)


#include <mpi.h>
#include <iostream>
#include <unistd.h>

#define SUBGROUP_SIZE 5
#define NUM_MSGS 5
#define ROOT 0
#define GROUP_START 0

int main (int argc, char** argv)
{
	int size, rank, send_buf, recv_buf;
	MPI_Status status;
    MPI_Request request;
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
        groupRanks[i] = GROUP_START + i;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    MPI_Group_incl(world_group, SUBGROUP_SIZE, groupRanks, &sub_group);
    MPI_Comm_create(MPI_COMM_WORLD, sub_group, &sub_comm);

    if (rank >= GROUP_START && rank < GROUP_START + SUBGROUP_SIZE) {
//        if (rank == ROOT)
//            sleep(1);
//        if (rank == 2)
//            sleep(1);
        MPI_Reduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_SUM, ROOT, sub_comm);
        MPI_Reduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_SUM, ROOT, sub_comm);
 //       std::cout << "[Test] " << rank << " finished reduction 1" << std::endl;
 //       MPI_Reduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_SUM, ROOT, sub_comm);
 //       std::cout << "[Test] " << rank << " finished reduction 2" << std::endl;
    }
    if (rank == ROOT)
        sleep(1);
//    MPI_Reduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
    MPI_Reduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
    MPI_Reduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);

	MPI_Finalize ();
	return 0;
}
