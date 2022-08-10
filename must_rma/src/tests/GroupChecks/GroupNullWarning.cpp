/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/GroupNullWarning \
// RUN: 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Warning:.*Argument.*[(]group1[)].*is MPI_GROUP_NULL, which is allowed but unusual}}

/**
 * @file GroupNullWarning.cpp
 * This is a test for the analysis group GroupChecks.
 *
 * Description:
 * Compares a group with MPI_GROUP_NULL, this will cause a warning.
 *
 *
 *  @date 25.05.2011
 *  @author Mathias Korepkat, Tobias Hilbrich
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

	//
	//create a new group
	MPI_Group group;
	MPI_Comm_group(MPI_COMM_WORLD,&group);

	int result;
	//create a communicator
	MPI_Group_compare(MPI_GROUP_NULL, group, &result);

	//free group
	if(group != MPI_GROUP_NULL)
		MPI_Group_free(&group);


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
