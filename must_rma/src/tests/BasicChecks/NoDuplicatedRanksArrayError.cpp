/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/NoDuplicatedRanksArrayError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]ranks[)].*array of ranks where no duplications are allowed.*entries are duplicated}}

/**
 * @file NoDuplicatedRanksArrayError.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Creates a new group with the help of the call Group_excl,
 * with duplication in rank array, what will cause an error.
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
	if (size < 2)
	{
		std::cerr << "This test needs at least 2 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

	MPI_Group group1, group2;
	MPI_Comm_group(MPI_COMM_WORLD,&group1);

	int n=2;
	int ranks[2];
	ranks[0] = 1;
	ranks[1] = 1;

	MPI_Group_excl(group1,n,ranks,&group2);


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
