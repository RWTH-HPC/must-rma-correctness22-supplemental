/*
 * Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: export TYPEART_TYPE_FILE=%must-bin-dir/%basename_t-o.yaml
// RUN: %typeart-mpicc -O2 %s -o %must-bin-dir/%basename_t.exe

// RUN: %must-run %mpiexec-numproc-flag 2 --must:typeart \
// RUN: %must-bin-dir/%basename_t.exe 2>&1 \
// RUN: | %filecheck %s --check-prefix CHECK-OPT

// RUN: export TYPEART_TYPE_FILE=%must-bin-dir/%basename_t-g.yaml
// RUN: %typeart-mpicc %s -o %must-bin-dir/%basename_t.exe

// RUN: %must-run %mpiexec-numproc-flag 2 --must:typeart \
// RUN: %must-bin-dir/%basename_t.exe 2>&1 \
// RUN: | %filecheck %s --check-prefix CHECK-G

// REQUIRES: typeart

#include <stddef.h>
#include <mpi.h>

typedef struct {
  float imag;
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

//  complex work;
  MPI_Bcast(cpt, 1, datatype, 0, MPI_COMM_WORLD);
//  *cpt = work;

  MPI_Type_free(&datatype);
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // The difference between OPT (-O2) and G (debug build) is that TypART tracks a
  // int64 instead of struct complex. Hence, the MUST TypeART checker fails, as the containing
  // type is not a struct.
  // CHECK-OPT: Distributed struct not yet implemented.
  // CHECK-G-NOT: Distributed struct not yet implemented.
  complex c={1.0, 2.0};  // This allocation becomes an int64 with a bit-mask store at -Ox
  g_complexsum(&c);

  MPI_Finalize();
  return 0;
}
