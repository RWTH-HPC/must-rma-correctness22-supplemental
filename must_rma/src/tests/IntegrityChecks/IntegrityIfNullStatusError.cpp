/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// For OpenMPI >=3 NULL status and MPI_STATUS_IGNORE are not passed to the analysis functions
// as a NULL pointer
// XFAIL: *
// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegrityIfNullStatusError 2>&1 \
// RUN: | %filecheck-may-segfault %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]status[)].*is a NULL pointer where an allocated memory region}}

/**
 * @file IntegrityIfNullStatusError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a send and recv. The address of the status in the recv call is a null pointer this will cause an error.
 *
 *  @date 03.06.2011
 *  @author Mathias Korepkat
 */

#include <iostream>
#include <mpi.h>

#include "mustConfig.h"

int main (int argc, char** argv)
{
	int size, rank;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	MPI_Status status;

	//Enough tasks ?
	if (size < 1)
	{
		std::cerr << "This test needs at least 1 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;


    //send
    if (rank == 0)
    {
        MPI_Send(&size, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
#ifdef HAVE_MPI_STATUS_IGNORE
        MPI_Send(&size, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
#endif /*HAVE_MPI_STATUS_IGNORE*/
    }

    //recv
    if (rank == 1)
    {
        MPI_Recv(&size, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, NULL); /* this will cause an error IF MPI_STATUS_IGNORE != NULL or not defined (MPI-1)*/
#ifdef HAVE_MPI_STATUS_IGNORE
        MPI_Recv(&size, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#endif /*HAVE_MPI_STATUS_IGNORE*/
    }

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
