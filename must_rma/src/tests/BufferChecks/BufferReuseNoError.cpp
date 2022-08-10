/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/BufferReuseNoError \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file BufferReuseNoError.cpp
 * This is a a test for the analysis BufferCheck.
 *
 * Description:
 * repeated: A buffer is attached to MPI, BufferedSend, buffer detached
 * Should cause no error
 * 
 *  @date 14.01.13
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

#define TEST_SEND_SIZE 1000000

int main (int argc, char** argv)
{
	int size,size1,size2, rank, i;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	//Enough tasks ?
	if (size < 2)
	{
		std::cerr << "This test needs at least 2 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    int* buffer = new int[TEST_SEND_SIZE];

	if (rank == 1)
	{
        int buffsize;
        MPI_Pack_size(TEST_SEND_SIZE, MPI_INT, MPI_COMM_WORLD, &buffsize);
        int* mpibuff = new int[(buffsize + MPI_BSEND_OVERHEAD - 1)/sizeof(int) +1];
        for (i=0; i<10; i++){
            MPI_Buffer_attach(mpibuff, buffsize + MPI_BSEND_OVERHEAD);
            MPI_Bsend(buffer, TEST_SEND_SIZE, MPI_INT, 0, 42, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Buffer_detach(&mpibuff, &buffsize);
        }
        delete[] mpibuff;
	}


	if (rank == 0)
	{
        for (i=0; i<10; i++){
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Recv(buffer, TEST_SEND_SIZE, MPI_INT, 1, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
	}
    delete[] buffer;
	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
