/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 --must:fanin 2 \
// RUN: %must-bin-dir/DnbcCollIbarrierWaitP2PDl 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Wait}}

/**
 * @file nbcCollIbarrierWaitP2PDl.c
 * A test that deadlocks in a wait call that either waits for an MPI_Ibarrier to complete or for a P2P operation (Deadlock).
 *
 * Description:
 * All processes execute an MPI_Ibarrier on the same communicator, excepting rank 0, which issues an MPI_Irecv instead, they then wait on the resulting requests and deadlock.
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

    //Rank 0 issues a P2P, the rest an IBarrier
    if (rank == 0)
    {
        MPI_Irecv (&size, 1, MPI_INT, 1, 666, MPI_COMM_WORLD, &request);
    }
    else
    {
        MPI_Ibarrier (MPI_COMM_WORLD, &request);
    }

    //Wait and deadlock
    MPI_Wait (&request, &status);

    //We now finish up the P2P communication (though we won't get here)
    if (rank == 1)
    {
        MPI_Send (&size, 1, MPI_INT, 0, 666, MPI_COMM_WORLD);
    }

    //Rank 0 may somewhere provide what would be needed (though we won't get here)
    if (rank == 0)
    {
        MPI_Ibarrier (MPI_COMM_WORLD, &request);
        MPI_Wait (&request, &status);
    }

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
