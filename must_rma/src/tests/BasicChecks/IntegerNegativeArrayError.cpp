/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegerNegativeArrayError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error.*Argument 3 [(]dims[)].*non-negative.*dims.1.=-1}}

/**
 * @file IntegerNegativeArrayError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Creates a cartesian communicator with a negative entry (-1) in the dims attribute,
 * which is a usage error.
 *
 *  @date 04.03.2011
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
	dims[1] = -1; /*!!!HERE COMES THE ERROR FROM*/
	dims[2] = -1;
	periods[0] = 1;
	periods[1] = 1;
	periods[2] = 1;

	//!!! Warning happens here
	MPI_Cart_create( MPI_COMM_WORLD, 3, dims /*!!!HERE THE ERROR IS PASSED TO MPI*/, periods, 0, &comm );
	MPI_Comm_free( &comm );

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
