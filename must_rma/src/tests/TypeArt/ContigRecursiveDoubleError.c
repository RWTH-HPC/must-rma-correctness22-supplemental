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

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Datatype mpi_double_vec;
  MPI_Type_contiguous(3, MPI_DOUBLE, &mpi_double_vec);
  MPI_Type_commit(&mpi_double_vec);

  MPI_Datatype mpi_double_arr;
  MPI_Type_contiguous(3, mpi_double_vec, &mpi_double_arr);
  MPI_Type_set_name(mpi_double_arr, "test_type");
  MPI_Type_commit(&mpi_double_arr);

  double f[8]; // Fix: should be length 9

  if (rank == 0) {
    // CHECK: Buffer too small: Transfer of type [1x"MUST_type_contiguous"] with byte count of 72 longer than buffer argument of type [8x"double"] with byte count of 64.
    MPI_Send(f, 1, mpi_double_arr, 1, 0, MPI_COMM_WORLD);
  } else if(rank == 1) {
    // CHECK: Buffer too small: Transfer of type [1x"MUST_type_contiguous"] with byte count of 72 longer than buffer argument of type [8x"double"] with byte count of 64.
    MPI_Recv(f, 1, mpi_double_arr, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Type_free(&mpi_double_arr);
  MPI_Type_free(&mpi_double_vec);

  MPI_Finalize();
  return 0;
}