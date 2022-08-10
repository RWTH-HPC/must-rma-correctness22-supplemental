/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/GroupNullError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]group[)].*is null, where a valid group was expected}}

/**
 * @file GroupNotKnownError.cpp
 * This is a test for the analysis group GroupChecks.
 *
 * Description:
 * Creates a communicator with a group set to MPI_GROUP_NULL. This will cause an error.
 *
 * Positiv check is: GroupNotKnownNoError
 *
 *  @date 23.05.2011
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


	MPI_Comm newcomm;
	MPI_Group group;

	group = MPI_GROUP_NULL;

	//create a communicator
	MPI_Comm_create(MPI_COMM_WORLD, group, &newcomm);

	//free group and communicator
	if(newcomm != MPI_COMM_NULL)
		MPI_Comm_free(&newcomm);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
