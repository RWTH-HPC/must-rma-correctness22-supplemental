/*
 * Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: export TYPEART_TYPE_FILE=%must-bin-dir/%basename_t-o.yaml
// RUN: %typeart-mpicc %s -o %must-bin-dir/%basename_t.exe

// RUN: %must-run %mpiexec-numproc-flag 2 --must:typeart \
// RUN: %must-bin-dir/%basename_t.exe 2>&1 \
// RUN: | %filecheck %s

// REQUIRES: typeart

#include <mpi.h>
#include <stddef.h>

typedef struct {
  float imag;
  float real;
} nested;

typedef struct {
  float imag;
  nested n;
  float real;
} complex;

void g_complexsum(complex* cpt) {
  int counts[2]         = {1, 1};
  MPI_Aint offsets[2]   = {offsetof(complex, imag), offsetof(complex, real)};
  MPI_Datatype types[2] = {MPI_FLOAT, MPI_FLOAT};

  MPI_Datatype datatype;
  MPI_Type_create_struct(2, counts, offsets, types, &datatype);
  MPI_Type_set_name(datatype, "struct_test_type");
  MPI_Type_commit(&datatype);

  // CHECK-NOT: Error
  MPI_Bcast(cpt, 1, datatype, 0, MPI_COMM_WORLD);

  MPI_Type_free(&datatype);
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  complex c={1.0, 2.0, 3.0, 4.0};
  g_complexsum(&c);

  MPI_Finalize();
  return 0;
}
