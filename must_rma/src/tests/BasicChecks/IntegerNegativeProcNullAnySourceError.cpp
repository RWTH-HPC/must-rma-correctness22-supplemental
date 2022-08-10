/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegerNegativeProcNullAnySourceError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]root[)].*is a rank that must be in the given communicator, but it is either negative, MPI_PROC_NULL, or MPI_ANY_SOURCE}}

/**
 * @file IntegerNegativeProcNullAnySourceError.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a MPI_Reduce with an root value of -1,
 * what causes an error.
 *
 *  @date 14.04.2011
 *  @author Mathias Korepkat
 */

#include <iostream>
#include <mpi.h>

int main (int argc, char** argv)
{
	int size, rank;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	MPI_Status status;

	//Enough tasks ?
	if (size < 2)
	{
		std::cerr << "This test needs at least 2 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

	int sum_of_ranks = 0;
	MPI_Reduce(&rank,&sum_of_ranks,1,MPI_INT,MPI_SUM,-1,MPI_COMM_WORLD);


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
