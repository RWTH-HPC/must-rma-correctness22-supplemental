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
 * @file VCCollBlockingAllToAll.cpp
 *       Blocking all-to-all communication example
 *
 *  @date 05.06.2021
 *  @author Felix Tomski
 */

// RUN: %must-run --must:layout \
// RUN: %builddir/tests/VectorClock/VCCollBlockingAllToAllLayout.xml \
// RUN: %must-bin-dir/VCCollBlockingAllToAll 2>&1 \
// RUN: | FileCheck %s

// CHECK-DAG: shutdown(0){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(1){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(2){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(3){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(4){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(5){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(6){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(7){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(8){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(9){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)
// CHECK-DAG: shutdown(10){{.*}}clk=(8, 10, 9, 11, 10, 10, 11, 11, 12, 12, 9, 9)

#include <iostream>
#include <mpi.h>
#include <unistd.h>

#define SUBGROUP_SIZE 10
#define GROUP_START 0
#define ROOT 1

int main(int argc, char** argv) {
    int size, rank, send_buf, recv_buf;
    MPI_Status status;
    MPI_Request request;
    MPI_Group world_group, sub_group;
    MPI_Comm sub_comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int groupRanks[SUBGROUP_SIZE];
    for (int i = 0; i < SUBGROUP_SIZE; i++)
        groupRanks[i] = GROUP_START + i;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    MPI_Group_incl(world_group, SUBGROUP_SIZE, groupRanks, &sub_group);
    MPI_Comm_create(MPI_COMM_WORLD, sub_group, &sub_comm);

    for (int i = 0; i < (rank / 2) + 1; i++) {
        if (rank % 2)
            MPI_Recv(&recv_buf, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        else
            MPI_Send(&send_buf, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < 2; i++) {
        if (rank == 1)
            MPI_Send(&send_buf, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
        if (rank == 3)
            MPI_Recv(&recv_buf, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

        if (rank >= GROUP_START && rank < SUBGROUP_SIZE + GROUP_START) {
            //        sleep(1);
            //        for (int i = 0; i < 10; i++)
            MPI_Bcast(&send_buf, 1, MPI_INT, ROOT, sub_comm);
            if (rank == 0)
                sleep(1);
            MPI_Barrier(sub_comm);
            //            MPI_Bcast(&send_buf, 1, MPI_INT, ROOT, sub_comm);
            //            MPI_Allreduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_MAX, sub_comm);
        }
        //    if (rank == 0) {
        //        sleep(1);
        //    }
        //    for (int i = 0; i < 2; i++)
        //        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Allreduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    int* temp = (int*) malloc(sizeof(int) * size);
    MPI_Allgather(temp, 1, MPI_INT, &recv_buf, 1, MPI_INT, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}
