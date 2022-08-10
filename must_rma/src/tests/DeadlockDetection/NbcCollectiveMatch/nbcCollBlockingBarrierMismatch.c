/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 5 --must:fanin 2 \
// RUN: %must-bin-dir/DnbcCollBlockingBarrierMismatch 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*The application matches a blocking collective .* with a non-blocking collective .*!}}

/**
 * @file nbcCollBlockingBarrierMismatch.c
 * This test case attempts to match an MPI_Ibarrier with an MPI_Barrier,
 * which is not allowed according to the MPI-3 standard.
 *
 * Description:
 * All processes execute an MPI_Ibarrier on MPI_COMM_WORLD,
 * they then wait on the resulting request.
 * However, process 1 executes an MPI_Barrier instead.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
    int rank,
         size;
    MPI_Status status;
    MPI_Request request;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    //Enough tasks ?
    if (size < 2)
    {
        printf ("This test needs at least 2 processes!\n");
        MPI_Finalize();
        return 1;
    }

    //Say hello
    printf ("Hello, I am rank %d of %d processes.\n", rank, size);

    if (rank == 1)
    {
        MPI_Ibarrier (MPI_COMM_WORLD, &request);
        MPI_Wait (&request, &status);
    }
    else
    {
        MPI_Barrier (MPI_COMM_WORLD);
    }

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
