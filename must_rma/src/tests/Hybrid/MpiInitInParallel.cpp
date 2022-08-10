/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// REQUIRES: hybrid
// RUN: %mustrun-hybrid %mpiexec-numproc-flag 4 \
// RUN: %must-bin-dir/MpiInitInParallel 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file CommNullError.cpp
 * This is a test for the analysis group CommChecks.
 *
 * Description:
 * Performs a send, recv with MPI_COMM_NULL as communicator in send call.
 * This will cause an error.
 *
 * Positiv check is: CommNotKnownNoError
 *
 *  @date 15.04.2011
 *  @author Mathias Korepkat
 */

#include <iostream>
#include <mpi.h>
#include <omp.h>

int main (int argc, char** argv)
{
  int size, rank, provided;


  #pragma omp parallel num_threads(2)
  {
    if (omp_get_thread_num()==0)
    {
      MPI_Init_thread (&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
      if (provided < MPI_THREAD_SERIALIZED){
        std::cout << "This test needs at least MPI_THREAD_SERIALIZED." << std::endl;
        MPI_Abort (MPI_COMM_WORLD,1);
      }
    }
    #pragma omp barrier
    if (omp_get_thread_num()==1)
    {
	    MPI_Comm_size(MPI_COMM_WORLD, &size);
	    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	    //Say hello
	    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;
    }
  }
  //Say bye bye
  std::cout << "Signing off, rank " << rank << "." << std::endl;

  MPI_Finalize ();

  return 0;
}
