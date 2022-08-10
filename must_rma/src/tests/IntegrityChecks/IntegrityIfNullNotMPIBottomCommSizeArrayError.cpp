/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// XFAIL: *
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegrityIfNullNotMPIBottomCommSizeArrayError 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]recvbuf[)].*is a NULL pointer where an allocated region was expected!}}

/**
 * @file IntegrityNullNotMPIBottomCommSizeArrayError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a MPI_Allgather with a recv buffer set to NULL a communicator > 0 and ranks defined in counts,
 * this will cause an error.
 *
 * Positiv check: IntegrityIfNullCommSizeNoError.cpp
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

    char          sendbuf[7] = {'I','T','W','o','r','k','s'},
    		      recvbuf[7] = {'I','T','W','o','r','k','s'};



	 MPI_Allgather(sendbuf,
			 1,
			 MPI_CHAR,
	         NULL,/* if the communicator is valid >0 and there are elements defined in counts,
	                  so this will cause an error */
	         1,
	         MPI_CHAR,
	         MPI_COMM_WORLD);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
