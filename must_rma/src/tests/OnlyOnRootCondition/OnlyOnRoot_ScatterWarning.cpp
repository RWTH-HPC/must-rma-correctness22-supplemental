/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/OnlyOnRoot_ScatterWarning 2>&1 \
// RUN: | %filecheck-may-segfault --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Warning:.*MPI_Scatter@.*0: Argument 2 \
// RUN: [(]sendcount[)] is zero,)}}' %s

// CHECK: [MUST-REPORT]{{.*Warning:.*MPI_Scatter@.*1: Argument 2 [(]sendcount[)] is zero,}}

/**
 * @file OnlyOnRoot_Scatter.cpp
 * This is a a test for the preconditioner of OnlyOnRootCondition for MPI_Scatter.
 *
 * Description:
 * Performs a Scatter with a sendcount set to 0,
 * which is allowed but suspicious. This is just significant at root.
 *
 *  @date 24.04.2011
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

	//create a cartesian communicator
	MPI_Comm comm;

	int inBuff = 1;
	int outBuff[2] = {0,0};

	MPI_Scatter(&inBuff,0,MPI_INT,outBuff,1,MPI_INT,1,MPI_COMM_WORLD);


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
