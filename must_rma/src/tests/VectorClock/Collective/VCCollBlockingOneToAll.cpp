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
 * @file VCCollBlockingOneToAll.cpp
 *       Blocking one-to-all communication example
 *
 *  @date 05.06.2021
 *  @author Felix Tomski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCCollBlockingOneToAllLayout.xml \
// RUN: %must-bin-dir/VCCollBlockingOneToAll 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
// CHECK-DAG: shutdown(6){{.*}}clk=(3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0)
// CHECK-DAG: shutdown(10){{.*}}clk=(3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0)

#include <mpi.h>
#include <iostream>
#include <unistd.h>

#define SUBGROUP_SIZE 10

int main (int argc, char** argv)
{
	int size, rank, send_buf, recv_buf, root=0;
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
        groupRanks[i] = (root+i) % size;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    MPI_Group_incl(world_group, SUBGROUP_SIZE, groupRanks, &sub_group);
    MPI_Comm_create(MPI_COMM_WORLD, sub_group, &sub_comm);
    
    if (rank >= root && rank < SUBGROUP_SIZE+root) {
        int sub_comm_root;
        MPI_Group_translate_ranks(world_group, 1, &root, sub_group, &sub_comm_root);
        if (rank == root) {
            MPI_Bcast(&send_buf, 1, MPI_INT, sub_comm_root, sub_comm);
//            MPI_Bcast(&send_buf, 1, MPI_INT, sub_comm_root, sub_comm);
        }
        else {
//            sleep(1);
            MPI_Bcast(&recv_buf, 1, MPI_INT, sub_comm_root, sub_comm);
//            MPI_Bcast(&recv_buf, 1, MPI_INT, sub_comm_root, sub_comm);
        }
    }

    if (rank == root) {
//        sleep(1);
        MPI_Bcast(&send_buf, 1, MPI_INT, root, MPI_COMM_WORLD);
        MPI_Bcast(&send_buf, 1, MPI_INT, root, MPI_COMM_WORLD);
    }
    else {
        MPI_Bcast(&recv_buf, 1, MPI_INT, root, MPI_COMM_WORLD);
        MPI_Bcast(&recv_buf, 1, MPI_INT, root, MPI_COMM_WORLD);
    }

	MPI_Finalize ();
	return 0;
}
