/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/GroupRangeStrideZeroError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]ranges[)].*triplets that contains zero for a stride, which must be greater or smaller than 0}}

/**
 * @file GroupRangeStrideZeroError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Creates a group with MPI_Group_range_excl with a stride of 0, which is not allowed
 *
 *  @date 21.03.2011
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


	MPI_Group group1,group2;
	int range[1][3];
	MPI_Comm_group( MPI_COMM_WORLD, &group1 );

	range[0][0] = 0;
	range[0][1] = 1;
	range[0][2] = 0; //this is not allowed

	//create a second group
	MPI_Group_range_excl( group1, 1, range, &group2 );

	MPI_Group_free(&group1);
	MPI_Group_free(&group2);


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
