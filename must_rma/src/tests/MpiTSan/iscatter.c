// REQUIRES: tsan

// RUN: rm -f %t/MUST_Output.html

// RUN: %mustrun-tsan --must:distributed %mpiexec-numproc-flag 2 \
// RUN: --must:output-dir %t --must:output html \
// RUN: %must-bin-dir/MpiTSanIscatter 2>&1  || true

// RUN: cat %t/MUST_Output.html | %filecheck %check-prefixes %s

// CHECK: {{.*Data race.*MPI_Iscatter}}

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <omp.h>

#define NUM_THREADS 12

int main(int argc, char* argv[])
{
  int provided;
  int* rbuf = (int*) malloc(NUM_THREADS/2*sizeof(int));
  int* sbuf = (int*) malloc(NUM_THREADS*sizeof(int));
  
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  
  if(provided < MPI_THREAD_FUNNELED)
  {
    printf("MPI level insufficient\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  
  MPI_Request req;
  
  int i;
  for(i = 0; i < 12; ++i) {
    sbuf[i] = i;
  }

#pragma omp parallel num_threads(NUM_THREADS)
 {  
#pragma omp master
   MPI_Iscatter(sbuf, NUM_THREADS/2, MPI_INT, rbuf, NUM_THREADS/2, MPI_INT, 0, MPI_COMM_WORLD, &req);
   // CHECK-ST-DAG: iscatter.c:[[@LINE+1]]
   sbuf[omp_get_thread_num()] = 5;
#pragma omp master
   // CHECK-ST-DAG: iscatter.c:[[@LINE+1]]
      MPI_Wait(&req, MPI_STATUS_IGNORE);
  }
  
  MPI_Finalize();
  
  return EXIT_SUCCESS;
}