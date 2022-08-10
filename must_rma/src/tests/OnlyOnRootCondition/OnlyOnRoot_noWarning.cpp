/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/OnlyOnRoot_noWarning \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file OnlyOnRoot_noWarning.cpp
 * This is a a test for the preconditioner of OnlyOnRootCondition for MPI_Scatter,MPI_Gather and MPI_Gatherv.
 *
 * Description:
 * Performs some collective Communication without any errors or warnings.
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
	int inBuff[2] = {1,1};
	int outBuff[2] = {0,0};
	int recvCounts[2]={1,1}, //a 0 in recv Count cause a warrning because it is unusual use.
	    recvDispl[2] = {0,1},
		recvDispl_err[2] = {0,-1};


	if(rank == 1)
		MPI_Scatter(&inBuff,1,MPI_INT,outBuff,1,MPI_INT,1,MPI_COMM_WORLD);
	else
		MPI_Scatter(&inBuff,0,MPI_INT,outBuff,1,MPI_INT,1,MPI_COMM_WORLD);

	if(rank == 1)
		MPI_Gather(&inBuff,1,MPI_INT,outBuff,0,MPI_INT,0,MPI_COMM_WORLD);
	else
		MPI_Gather(&inBuff,1,MPI_INT,outBuff,1,MPI_INT,0,MPI_COMM_WORLD);

	if(rank == 1)
		MPI_Gatherv(&inBuff,1,MPI_INT,outBuff,recvCounts,recvDispl_err,MPI_INT,0,MPI_COMM_WORLD);
	else
		MPI_Gatherv(&inBuff,1,MPI_INT,outBuff,recvCounts,recvDispl,MPI_INT,0,MPI_COMM_WORLD);


	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
