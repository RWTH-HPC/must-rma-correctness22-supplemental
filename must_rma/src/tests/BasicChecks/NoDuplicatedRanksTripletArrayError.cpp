/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 4 \
// RUN: %must-bin-dir/NoDuplicatedRanksTripletArrayError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]ranges[)].*array of triplets.*where no duplication of ranks is allowed.*contain equal ranks}}

/**
 * @file NoDuplicatedRanksTripletArrayError.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Creates a new group with the help of the call MPI_Group_range_excl,
 * with overlapping rank triplets in range array. This will cause an error.
 *
 *  @date 24.05.2011
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
	if (size < 4)
	{
		std::cerr << "This test needs at least 4 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

	MPI_Group group1, group2;
	MPI_Comm_group(MPI_COMM_WORLD,&group1);

	int n=3;
	int range[3][3] = {{0,3,2},{3,0,-2},{1,3,1}};
	MPI_Group_range_excl(group1,n,range,&group2);


	//free groups
	if(group1 != MPI_GROUP_NULL)
	{
		MPI_Group_free(&group1);
	}

	if(group2 != MPI_GROUP_NULL)
	{
		MPI_Group_free(&group2);
	}

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
