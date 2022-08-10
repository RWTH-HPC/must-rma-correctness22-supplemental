// Originally from TypeART library test base
/*
 * Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: export TYPEART_TYPE_FILE=%must-bin-dir/%basename_t.yaml
// RUN: %typeart-mpicc -O2 %s -o %must-bin-dir/%basename_t.exe

// RUN: %must-run %mpiexec-numproc-flag 2 --must:typeart \
// RUN: %must-bin-dir/%basename_t.exe 2>&1 \
// RUN: | %filecheck %s

// REQUIRES: typeart

// XFAIL: *

#include <mpi.h>

typedef struct {
  float imag;
  float real;
} complex;

void g_complexsum(complex* cpt) {
  complex work;
  MPI_Allreduce(cpt, &work, 2, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
  *cpt = work;
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // CHECK-NOT: Incompatible buffer of type 3 (int64)
  complex c={1.0, 2.0};  // This allocation becomes an int64 with a bit-mask store
  g_complexsum(&c);

  MPI_Finalize();
  return 0;
}