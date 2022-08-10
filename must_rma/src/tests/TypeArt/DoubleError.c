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
#include <stdlib.h>

#define ARRAY_LENGTH 11

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    double* a = (double*) malloc(ARRAY_LENGTH * sizeof(double));
    // CHECK: call MPI_Send@0: Incompatible buffer of type 6 (double) - expected MPI_FLOAT
    MPI_Send(a, ARRAY_LENGTH, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
    free(a);
  } else if(rank == 1) {
    float* b = (float*) malloc(ARRAY_LENGTH * sizeof(float));
    MPI_Recv(b, ARRAY_LENGTH, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    free(b);
  }

  MPI_Finalize();
  return 0;
}