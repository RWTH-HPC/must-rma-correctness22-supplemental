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

// CHECK: MUST detected no MPI usage errors

#include <mpi.h>
#include <stdlib.h>

#define ARRAY_LENGTH 11

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double* a = (double*) malloc(ARRAY_LENGTH * sizeof(double));
  if (rank == 0) {
    MPI_Send(a, ARRAY_LENGTH, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
  } else if(rank == 1) {
    MPI_Recv(a, ARRAY_LENGTH, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  free(a);
  MPI_Finalize();
  return 0;
}