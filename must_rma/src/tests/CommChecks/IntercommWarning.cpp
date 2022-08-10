/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/IntercommWarning \
// RUN: 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Warning:.*Argument.*}}

/**
 * @file IntercommWarning.cpp
 * This is a test for the analysis group CommChecks.
 *
 * Description:
 * Performs a topology test with an intercommunicator.
 * This will cause a warning.
 *
 *  @date 27.04.2011
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

	MPI_Comm intercomm,localcomm;
	int status,color;

	//Enough tasks ?
	if (size < 2)
	{
		std::cerr << "This test needs at least 2 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;


	//create a second intracommunicator
    color = rank % 2;
    MPI_Comm_split( MPI_COMM_WORLD, color, rank, &localcomm );

    //create a intercommunicator
    MPI_Intercomm_create( localcomm, 0, MPI_COMM_WORLD, 1-color, 42, &intercomm);

    //test the topologie of the intercommunicator this will cause a warning.
    MPI_Topo_test(intercomm,&status);

    MPI_Comm_free(&intercomm);
    MPI_Comm_free(&localcomm);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
