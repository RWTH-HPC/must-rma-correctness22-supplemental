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
#define ARRAY_LENGTH_CHAR 11*sizeof(char)*sizeof(double)

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    // CHECK:  Buffer too small: Transfer of type [88x"MPI_BYTE"] with byte count of 88 longer than buffer argument of type [10x"double"] with byte count of 80.
    double* a = (double*) malloc(ARRAY_LENGTH * sizeof(double));
    MPI_Send(&a[1], ARRAY_LENGTH_CHAR, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
    free(a);
  } else if(rank == 1) {
    char* b = (char*) malloc(ARRAY_LENGTH_CHAR);
    MPI_Recv(b, ARRAY_LENGTH_CHAR, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    free(b);
  }

  MPI_Finalize();
  return 0;
}