/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/IntegerNegativeError \
// RUN: 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error.*count.*is negative}}

/**
 * @file IntegerNegativeError.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a send, recv where the send uses a count of -1, which is a
 * usage error.
 *
 *  @date 01.03.2011
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

	if (rank == 1)
	{
		MPI_Send (&size, -1, MPI_INT, 1, 42, MPI_COMM_WORLD);
	}

	if (rank == 0)
	{
		MPI_Recv (&size, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, &status);

	}

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
