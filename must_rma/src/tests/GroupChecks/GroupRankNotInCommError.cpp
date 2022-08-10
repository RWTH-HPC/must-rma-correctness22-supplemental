/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 4 \
// RUN: %must-bin-dir/GroupRankNotInCommError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]group[)].*group which should be a subset of argument.*ranks are in the group but not in the communicator}}

/**
 * @file GroupRankNotInCommNoError.cpp
 * This is a test for the analysis group GroupChecks.
 *
 * Description:
 * Creates a communicator with the help of Comm_create call,
 * but has more ranks in group then in communicator. This will cause an error.
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
	if (size < 4)
	{
		std::cerr << "This test needs at least 4 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

	//
	//create a new group
	MPI_Comm newcomm,oldcomm;
	MPI_Group group,tmpgroup;

	int n = 1;
	int range[1][3] = {{3,0,-1}};
	MPI_Comm_group(MPI_COMM_WORLD,&tmpgroup);

	MPI_Group_range_incl(tmpgroup,1,range,&group);

	//create a communicator to build a new communicator with
	int order = 3-rank;
	if(rank == 2)
	{
		MPI_Comm_split(MPI_COMM_WORLD,2,0,&oldcomm); //this trank is in group but not in the communicator
	}
	else
	{
		if(order==3) order= 1;
		MPI_Comm_split(MPI_COMM_WORLD,1,order,&oldcomm);
	}

	//create a communicator
	// this will cause an error. rank 2 is in group but not in oldcomm
	MPI_Comm_create(oldcomm, group, &newcomm);

	//free group and communicator
	if(newcomm != MPI_COMM_NULL)
		MPI_Comm_free(&newcomm);

	if(oldcomm != MPI_COMM_NULL)
		MPI_Comm_free(&oldcomm);

	if(tmpgroup != MPI_GROUP_NULL)
		MPI_Group_free(&tmpgroup);

	if(group != MPI_GROUP_NULL)
		MPI_Group_free(&group);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
