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
    double a;
    double b;
} VecT;

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Datatype vec_struct_ty;
  int length[2]         = {1, 1};
  MPI_Aint offsets[2]   = {offsetof(VecT, a), offsetof(VecT, b)};
  MPI_Datatype types[2] = {MPI_DOUBLE, MPI_DOUBLE};
  MPI_Type_create_struct(2, length, offsets, types, &vec_struct_ty);

  MPI_Datatype hvec_type;
  MPI_Type_vector(3, 1, 1, vec_struct_ty, &hvec_type);
  MPI_Type_commit(&hvec_type);

  VecT buffer[3] = {1., 2., 3., 4., 5., 6.};

  if (rank == 0) {
    // CHECK: Buffer too small: Transfer of type [2x"MUST_type_vector"] with byte count of 96 longer than buffer argument of type [3x"struct.VecT"] with byte count of 48.
    MPI_Send(&buffer[0], 2, hvec_type, 1, 0, MPI_COMM_WORLD);
  } else if(rank == 1) {
    // CHECK: Buffer too small: Transfer of type [2x"MUST_type_vector"] with byte count of 96 longer than buffer argument of type [3x"struct.VecT"] with byte count of 48.
    MPI_Recv(&buffer[0], 2, hvec_type, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Type_free(&vec_struct_ty);
  MPI_Type_free(&hvec_type);
  MPI_Finalize();
  return 0;
}
