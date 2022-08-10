/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 4 %must-bin-dir/CommNotGraphNoError \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file CommNotGraphNoError.cpp
 * This is a a test for the analysis group CommChecks.
 *
 * Description:
 * Creates a graph communicator and performs a call to MPI_Graph_neighbors_count
 * on rank 0 without any errors or warnings.
 *
 *
 *  @date 27.04.2011
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
	int count;

	MPI_Graph_create( MPI_COMM_WORLD, 4, index, edges, 0, &comm1 );

	if(rank == 0)
	{
		MPI_Graph_neighbors_count(comm1,0,&count);
	}

	if(comm1 != MPI_COMM_NULL)
		MPI_Comm_free(&comm1);


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
