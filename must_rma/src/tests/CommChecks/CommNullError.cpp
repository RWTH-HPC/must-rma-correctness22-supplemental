/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/CommNullError 2>&1 \
// RUN: | %filecheck-may-segfault %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]comm[)].*is MPI_COMM_NULL.}}

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


	MPI_Comm comm;
	comm = MPI_COMM_NULL;
	if (rank == 0)
	{
		MPI_Send (&size, 1, MPI_INT, 1, 42, comm);
	}

	if (rank == 1)
	{
		MPI_Recv (&size, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, &status);

	}

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
