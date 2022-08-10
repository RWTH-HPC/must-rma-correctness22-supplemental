/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/RequestNullNoWarning \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file RequestNullNoWarning.cpp
 * This is a a test for the analysis RequestCheck.
 *
 * Description:
 * Performs a irecv,send with a wait.
 * The Request that is given to the wait call is not set to NULL,
 * so there will be no errors or warnings
 *
 *
 *  @date 06.04.2011
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
	MPI_Request request;
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
		MPI_Irecv (&size, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, &request);
		MPI_Wait(&request,&status);
	}


	if (rank == 0)
	{
		MPI_Send (&size, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
	}

	if(request != MPI_REQUEST_NULL && rank == 1)
		MPI_Request_free (&request);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
