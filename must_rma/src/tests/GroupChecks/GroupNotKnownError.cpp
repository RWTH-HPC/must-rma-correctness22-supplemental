/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/GroupNotKnownError \
// RUN: 2>&1 \
// RUN: | %filecheck-may-segfault %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]group[)].*is an unknown group}}

/**
 * @file GroupNotKnownError.cpp
 * This is a test for the analysis group GroupChecks.
 *
 * Description:
 * Creates a communicator with a unknown group. This will cause an error.
 *
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

	//
	//create a new group
	MPI_Comm newcomm;
	MPI_Group tmpgroup,group;

	//create a unknown group
	MPI_Comm_group(MPI_COMM_WORLD,&tmpgroup);
	group = tmpgroup;
	MPI_Group_free(&tmpgroup);

	//create a communicator
	MPI_Comm_create(MPI_COMM_WORLD, group, &newcomm);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
