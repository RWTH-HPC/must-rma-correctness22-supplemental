// REQUIRES: tsan
// REQUIRES: tsan-tlc

// RUN: rm -f %t/MUST_Output.html

// RUN: %mustrun-tsan %mpiexec-numproc-flag 2 \
// RUN: --must:output-dir %t --must:output html \
// RUN: %must-bin-dir/MpiTSanIsendSingle 2>&1  || true

// RUN: cat %t/MUST_Output.html | %filecheck %check-prefixes %s

// CHECK: {{.*Data race}}

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <omp.h>

#define NUM_ELEM 2

int main(int argc, char* argv[])
{
  int provided;
  int *rdata, *sdata, rank, size;
  rdata = malloc(NUM_ELEM * sizeof(rdata));
  sdata = malloc(NUM_ELEM * sizeof(sdata));
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  if(provided < MPI_THREAD_FUNNELED)
  {
    printf("MPI level insufficient\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if(rank == 0)
  {
    MPI_Request req2;
    MPI_Isend(sdata, NUM_ELEM, MPI_INT, size-rank-1, 42, MPI_COMM_WORLD, &req2);
    printf("Write to &sdata[0] = %p\n", &(sdata[0]));
    // CHECK-ST-DAG: isend_single.c:[[@LINE+1]]
    sdata[0] = 42;
    sdata[1] = 43;
    MPI_Wait(&req2, MPI_STATUS_IGNORE);
  }
  else if(rank == 1)
  {
    MPI_Recv(rdata, NUM_ELEM, MPI_INT, size-rank-1, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  printf("Write to sdata[0] = %i\n", sdata[0]);
  MPI_Finalize();
  return EXIT_SUCCESS;
}