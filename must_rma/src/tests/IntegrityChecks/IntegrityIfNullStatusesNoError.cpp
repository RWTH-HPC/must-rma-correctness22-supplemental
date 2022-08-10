/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/IntegrityIfNullStatusesNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file IntegrityIfNullStatusesNoError.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Performs a send and Irecv to get a request array. This request array will be tested with testall.
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

    //Enough tasks ?
    if (size < 2)
    {
        std::cerr << "This test needs at least 2 processes!"<< std::endl;
        MPI_Finalize();
        return 1;
    }

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Status statuses[4];
    MPI_Request request[4];
    int buffer[4],flag;

    if (rank == 1)
    {
        for(int i=0; i<4;i++)
        {
            MPI_Irecv(&(buffer[i]), 1, MPI_INT, 0, (42+i), MPI_COMM_WORLD, &(request[i]));
        }

        flag = 0;
        //MPI_Testall(4, request, &flag, statuses);
#ifdef HAVE_MPI_STATUSES_IGNORE
        MPI_Waitall(4, request, MPI_STATUSES_IGNORE);
#else
        MPI_Waitall(4, request, statuses);
#endif /*HAVE_MPI_STATUSES_IGNORE*/
    }


    if (rank == 0)
    {
        for(int i=0; i<4;i++)
        {
            buffer[i] = i;
            MPI_Send(&(buffer[4]), 1, MPI_INT, 1, (42+i), MPI_COMM_WORLD);
        }
    }

    //Say bye bye
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
