/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/RequestPersistentButInactiveError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]request[)].*is a persistent but in-active request!}}

/**
 * @file RequestPersistentButInactiveError.cpp
 * This is a a test for the analysis RequestCheck.
 *
 * Description:
 * Performs a MPI_Send_init and Cancels the request before call MPI_Start,
 * this causes a
 *
 *
 *  @date 08.04.2011
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

	if (rank == 0)
	{
		MPI_Send_init (&size, 1, MPI_INT, 1, 42, MPI_COMM_WORLD, &request);
		MPI_Cancel(&request);
		MPI_Start(&request);
	}

	if(rank == 1)
	{
		MPI_Irecv(&size, 1, MPI_INT,0,42,MPI_COMM_WORLD,&request);
	}

	MPI_Wait(&request,&status);
	if(request != MPI_REQUEST_NULL)
		MPI_Request_free (&request);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
