/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 4 \
// RUN: %must-bin-dir/MaxEdgesToSmallWarning 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Warning:.*Argument.*[(]maxedges[)].*graph topology, but is smaller then the number of edges in the given communicator}}

/**
 * @file MaxEdgesToSmallWarning.cpp
 * This is a a test for the analysis group CommChecks.
 *
 * Description:
 * Creates a graph communicator and performs a call to MPI_Graph_get
 * on rank 0 with a maxedges argument that is less then the number of edges in the communicator.
 * This will cause a warning.
 *
 * Positiv check is MaxIndicesToSmallNoWarning.cpp
 *
 *  @date 29.04.2011
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

	MPI_Comm comm1;
	int index[4] = {2,3,4,6};
	int edges[6] = {1,3,0,3,0,2};

	MPI_Graph_create( MPI_COMM_WORLD, 4, index, edges, 0, &comm1 );

	if(rank == 0)
	{
		MPI_Graph_get(comm1,4,3,index,edges);
	}

	if(comm1 != MPI_COMM_NULL)
		MPI_Comm_free(&comm1);


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
