/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/RequestNotKnownError \
// RUN: 2>&1 \
// RUN: | %filecheck-may-segfault %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]request[)].*is a unknown request}}

/**
 * @file RequestNotKnownError.cpp
 * This is a a test for the analysis RequestCheck.
 *
 * Description:
 * Performs a irecv,send with a wait but the wait get a unknown request,
 * which is a usage error.
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
		MPI_Status status;
		MPI_Request request,request_unknown;

		MPI_Irecv (&size, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, &request);
		int flag;

		request_unknown = request;
		MPI_Wait(&request,&status);
		MPI_Test(&request_unknown /* this is not the same as request and it is not known*/,&flag,&status);
		if(request_unknown != MPI_REQUEST_NULL)
			MPI_Request_free (&request_unknown);
	}


	if (rank == 0)
	{
		MPI_Send (&size, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
	}


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
