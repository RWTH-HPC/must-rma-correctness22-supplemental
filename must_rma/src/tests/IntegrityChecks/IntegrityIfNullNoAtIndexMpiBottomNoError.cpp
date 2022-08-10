/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegrityIfNullNoAtIndexMpiBottomNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not '[MUST-REPORT]{{.*(Error|ERROR)}}' %s

/**
 * @file IntegrityIfNullNoAtIndexMpiBottomNoError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a Reduce_scatter call, without triggering any errors or warnings.
 *
 *  @date 30.05.2011
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
	if (size != 2)
	{
		std::cerr << "This test needs exactly 2 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;


	int inBuff[2] = {4,6},
		recvCounts[2]={0,1};
	int outBuff = 0;

	if(rank == 0)
	{
		MPI_Reduce_scatter(inBuff,NULL,recvCounts,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	}
	else
	{
		MPI_Reduce_scatter(inBuff,&outBuff,recvCounts,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	}
	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
