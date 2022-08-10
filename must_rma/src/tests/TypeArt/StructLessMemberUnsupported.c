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

#include <stddef.h>
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

  int counts[2]         = {2, 1};
  MPI_Aint offsets[2]   = {offsetof(S1, a), offsetof(S1, b)};
  MPI_Datatype types[2] = {MPI_DOUBLE, MPI_INT};

  MPI_Datatype datatype;
  MPI_Type_create_struct(2, counts, offsets, types, &datatype);
  MPI_Type_set_name(datatype, "struct_test_type");
  MPI_Type_commit(&datatype);

  S1 s1;

  if (rank == 0) {
    // CHECK: Not yet implemented: Extent of struct differs from MPI extent.
    MPI_Send(&s1, 1, datatype, 1, 0, MPI_COMM_WORLD);
  } else if(rank == 1) {
    // CHECK: Not yet implemented: Extent of struct differs from MPI extent.
    MPI_Recv(&s1, 1, datatype, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Type_free(&datatype);

  MPI_Finalize();
  return 0;
}
