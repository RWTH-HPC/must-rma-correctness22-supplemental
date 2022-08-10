/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegerNegativeNotUndefinedError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]color[)].*non-negative integer or MPI_Undefined but it is:}}

/**
 * @file IntegerNegativeNotUndefinedError.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a MPI_Comm_split with a color attribute
 * set to a negative value that is not MPI_UNDEFINED.
 * This causes an error.
 *
 *  @date 14.04.2011
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

	MPI_Comm newcomm;
	int color = -1;
	while(color == MPI_UNDEFINED)
		color--;
	MPI_Comm_split(MPI_COMM_WORLD,color,rank,&newcomm);
	MPI_Comm_free(&newcomm);
	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
