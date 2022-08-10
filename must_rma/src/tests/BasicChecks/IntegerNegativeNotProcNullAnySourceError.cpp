/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegerNegativeNotProcNullAnySourceError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]source[)].*has to be a rank in the given communicator, MPI_PROC_NULL, or MPI_ANY_SOURCE, but is a negative value}}

/**
 * @file IntegerNegativeNotProcNullAnySourceError.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a send, recv with a negative integer as source in the
 * recv call. Thats not allowed and causes an error.
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
		MPI_Send (&size, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
	}

	if (rank == 1)
	{
		int recvFrom = -1;
		while(recvFrom == MPI_PROC_NULL || recvFrom == MPI_ANY_SOURCE)
			recvFrom--;
		MPI_Recv (&size, 1, MPI_INT, recvFrom, 42, MPI_COMM_WORLD, &status);

	}

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
