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
// UNSUPPORTED: typeart

// CHECK: MUST detected no MPI usage errors

#include <mpi.h>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Datatype mpi_subarray;
  int elems[1] = {9};
  int subarray_elems[1] = {6};
  int coords[1] = {3};
  MPI_Type_create_subarray(1, elems, subarray_elems, coords, MPI_ORDER_C, MPI_DOUBLE, &mpi_subarray);
  MPI_Type_set_name(mpi_subarray, "subarray_test_type");
  MPI_Type_commit(&mpi_subarray);

  double f[9];

  if (rank == 0) {
    MPI_Send(f, 1, mpi_subarray, 1, 0, MPI_COMM_WORLD);
  } else if(rank == 1) {
    MPI_Recv(f, 1, mpi_subarray, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Type_free(&mpi_subarray);
  MPI_Finalize();
  return 0;
}