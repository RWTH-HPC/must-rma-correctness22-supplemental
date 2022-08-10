/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 %must-bin-dir/CommIsNullNoWarning \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file CommIsNullNoWarning.cpp
 * This is a test for the analysis group  CommChecks.
 *
 * Description:
 * Performs a MPI_Comm_compare on MPI_COMM_WORLD without triggering any errors or warnings.
 *
 *  @date 28.04.2011
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

	//Enough tasks ?
	if (size < 3)
	{
		std::cerr << "This test needs at least 3 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;


	//create a second intracommunicator
	MPI_Comm comm;
	MPI_Comm_split(MPI_COMM_WORLD,1,rank,&comm);
    int res=0;
    MPI_Comm_compare(MPI_COMM_WORLD,comm,&res);
	if(comm != MPI_COMM_NULL)
		MPI_Comm_free(&comm);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
