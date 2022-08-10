/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegerLessTagUbWarning 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Warning:.*Argument.*[(]tag[)].*is a tag that is larger than 32767 and may thus only be supported by some MPI implementations}}

/**
 * @file IntegerLessTagUbWarning.cpp
 * This is a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a send with a tag set to 32769, what is higher then
 * the MPI standart require (32767) for the maximum tag. This will cause a
 * warning.
 *
 *
 *  @date 13.04.2011
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

	if (rank == 0)
	{
		MPI_Send (&size, 1, MPI_INT, 1, 32769, MPI_COMM_WORLD);
	}

	if (rank == 1)
	{
		MPI_Recv (&size, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	}

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
