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

  MPI_Datatype mpi_byte_vec;
  MPI_Type_contiguous(4, MPI_BYTE, &mpi_byte_vec);
  MPI_Type_set_name(mpi_byte_vec, "test_type_contiguous");
  MPI_Type_commit(&mpi_byte_vec);

  float f[2]; // Should be length 3

  if (rank == 0) {
    // CHECK: Buffer too small: Transfer of type [3x"MUST_type_contiguous"] with byte count of 12 longer than buffer argument of type [2x"float"] with byte count of 8
    MPI_Send(f, 3, mpi_byte_vec, 1, 0, MPI_COMM_WORLD);
  } else if(rank == 1) {
    // CHECK: Buffer too small: Transfer of type [3x"MUST_type_contiguous"] with byte count of 12 longer than buffer argument of type [2x"float"] with byte count of 8
    MPI_Recv(f, 3, mpi_byte_vec, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Type_free(&mpi_byte_vec);
  MPI_Finalize();
  return 0;
}