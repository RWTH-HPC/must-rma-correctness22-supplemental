/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// XFAIL: *
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegrityIfNullNoAtIndexMpiBottomError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]recvbuf[)].*is a NULL pointer where an allocated memory region}}

/**
 * @file IntegrityIfNullNoAtIndexMpiBottomError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a Reduce_scatter call with recvCounts set to NULL where a recv is expected.
 * This will cause an error.
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
	if (size < 2)
	{
		std::cerr << "This test needs at least 2 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;


	int inBuff[2] = {4,6},
		recvCounts[2]={1,1},
		outBuff[2] = {4,6};

	MPI_Reduce_scatter(inBuff,NULL,recvCounts,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
