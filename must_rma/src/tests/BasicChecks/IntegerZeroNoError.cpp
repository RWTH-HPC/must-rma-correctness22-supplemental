/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/IntegerZeroNoError \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file IntegerZeroNoError.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Creates a cartesian communicator and performs an MPI_Cart_shift
 * without any errors.
 *
 *  @date 03.03.2011
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

	//create a cartesian communicator
	MPI_Comm comm;
	int dims[3], periods[3];
	int source, dest;

	dims[0] = size;
	dims[1] = 1;
	dims[2] = 1;
	periods[0] = 1;
	periods[1] = 1;
	periods[2] = 1;

	MPI_Cart_create( MPI_COMM_WORLD, 3, dims, periods, 0, &comm );
	MPI_Cart_shift( comm, 0, 1, &source, &dest );
	MPI_Comm_free( &comm );

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
