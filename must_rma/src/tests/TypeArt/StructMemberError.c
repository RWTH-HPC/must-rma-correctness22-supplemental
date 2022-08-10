/*
 * Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: export TYPEART_TYPE_FILE=%must-bin-dir/%basename_t.yaml
// RUN: %typeart-mpicc %s -o %must-bin-dir/%basename_t.exe

// RUN: %must-run %mpiexec-numproc-flag 2 --must:typeart \
// RUN: %must-bin-dir/%basename_t.exe 2>&1 \
// RUN: | %filecheck %s

// REQUIRES: typeart

#include <mpi.h>

typedef struct {
  double a[2];
  int b;
  double c;
} S1;

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  S1 s1;

  if (rank == 0) {
    // CHECK: Incompatible buffer of type 2 (int32) - expected MPI_FLOAT
    MPI_Send(&s1.b, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
  } else if(rank == 1) {
    // CHECK: Incompatible buffer of type 2 (int32) - expected MPI_FLOAT
    MPI_Recv(&s1.b, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();
  return 0;
}