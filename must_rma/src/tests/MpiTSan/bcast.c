// REQUIRES: tsan

// RUN: rm -f %t/MUST_Output.html

// RUN: %mustrun-tsan %mpiexec-numproc-flag 2 \
// RUN: --must:output-dir %t --must:output html \
// RUN: %must-bin-dir/MpiTSanBcast 2>&1 || true

// RUN: cat %t/MUST_Output.html | %filecheck %check-prefixes %s

// CHECK: {{.*Data race.*MPI_Bcast}}

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <omp.h>

#define NUM_THREADS 12

int main(int argc, char* argv[])
{
  int provided;
  int* buf = (int*) malloc(12*sizeof(int));
  
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  
  if(provided < MPI_THREAD_FUNNELED)
  {
    printf("MPI level insufficient\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  
  MPI_Request req;
  
  int i;
  for(i = 0; i < 12; ++i) {
    buf[i] = i;
  }

#pragma omp parallel num_threads(NUM_THREADS)
  {
    // CHECK-ST-DAG: bcast.c:[[@LINE+1]]
    buf[omp_get_thread_num()] = 4;
#pragma omp master
    {
      // CHECK-ST-DAG: bcast.c:[[@LINE+1]]
      MPI_Bcast(buf, NUM_THREADS, MPI_INT, 0, MPI_COMM_WORLD);
    }
  }
  
  MPI_Finalize();
  
  return EXIT_SUCCESS;
}