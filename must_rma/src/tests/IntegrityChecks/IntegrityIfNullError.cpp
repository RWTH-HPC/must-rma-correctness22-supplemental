/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/IntegrityIfNullError \
// RUN: 2>&1 \
// RUN: | %filecheck-may-segfault %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]inbuf[)] is a NULL pointer where an allocated memory region with a size}}

/**
 * @file IntegrityIfNullError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Packs a message on rank 0, sends this package to rank 1 and unpack it there.
 * the buffer that will be unpacked, is set to NULL, what will cause an error.
 *
 *  @date 27.05.2011
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
	if (size < 1)
	{
		std::cerr << "This test needs at least 1 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

	char myArray[30];
	char buffer[50];
	int position = 0;

	//Pack a message (myArray) and send it to rank 1
    if (rank == 0)
    {
        for (int i=0; i<30; i++)
            myArray[i] = (char) (48+i);

        MPI_Pack(myArray, 30, MPI_CHAR, buffer, 50, &position, MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, 1, 42, MPI_COMM_WORLD);
    }

    //Recv a package and unpack it
    if (rank == 1)
    {
        MPI_Recv(buffer, 50, MPI_PACKED, 0, 42, MPI_COMM_WORLD, &status);
        MPI_Unpack(NULL /*this will cause an error */, 50, &position, myArray, 30, MPI_CHAR, MPI_COMM_WORLD);
    }

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
