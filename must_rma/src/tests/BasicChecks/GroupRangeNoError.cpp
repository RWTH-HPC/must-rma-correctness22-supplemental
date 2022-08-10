/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/GroupRangeNoError \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file GroupRangeNoError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Creates a group with MPI_Group_range_excl without any error or warning
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
	range[0][2] = 1;

	//create a second group
	MPI_Group_range_excl( group1, 1, range, &group2 );

	MPI_Group_free(&group1);
	MPI_Group_free(&group2);


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
