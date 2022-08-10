/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegerNegativeNotProcNullError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]dest[)].*has to be a rank in the given communicator or MPI_PROC_NULL, but is negative}}

/**
 * @file IntegerNegativeNotProcNullError.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a send, recv and uses a negative integer that is not MPI_PROC_NULL
 * as dest argument. This causes an error.
 *
 *  @date 13.04.2011
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

	if (rank == 0)
	{
		int sendTo = -1;
		while(sendTo == MPI_PROC_NULL)
			sendTo--;
		MPI_Send (&size, 1, MPI_INT, sendTo, 42, MPI_COMM_WORLD);
	}

	if (rank == 1)
	{

		MPI_Recv (&size, 1, MPI_INT, MPI_ANY_SOURCE, 42, MPI_COMM_WORLD, &status);

	}

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
