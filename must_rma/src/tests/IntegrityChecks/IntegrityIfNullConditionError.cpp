/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegrityIfNullConditionError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]periods[)] is a NULL pointer where an allocated memory region}}

/**
 * @file IntegrityIfNullConditionError.cpp
 *
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Creates a cartesian communicator with the periods argument set to null,
 * what will cause an error.
 *
 *  @date 27.05.2011
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
	int dims[2];
	int source, dest;
	dims[0] = 2;
	dims[1] = 1;

	//create a cartesian Communicator
	MPI_Cart_create( MPI_COMM_WORLD, 2, dims, NULL /* this will cause an error*/, 0, &comm );

	//free the communicator
	if(comm != MPI_COMM_NULL)
		MPI_Comm_free( &comm );

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
