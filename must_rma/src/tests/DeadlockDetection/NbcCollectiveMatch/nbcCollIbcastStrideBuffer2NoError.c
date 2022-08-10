/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 --must:fanin 2 \
// RUN: %must-bin-dir/DnbcCollIbcastStrideBuffer2NoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file nbcCollIreduceStrideBuffer2NoError.c
 * A test with two MPI_Ibcast calls on each rank, followed by an MPI_Waitall
 * and a repetition of the same communication.
 * The ranks use strides to avoid an overlap between the buffers (No Error).
 *
 * Description:
 * Each process executes two MPI_Ibcast calls, then waits for both of them to complete
 * and then repeats this pattern.
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
    MPI_Status statuses[2];
    MPI_Request requests[2];
    MPI_Datatype vectorType;
    int i, index;
    int buf[4] = {7, 11, 3, 1};

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

    //Create a strided MPI datatype
    MPI_Type_vector (2, 1, 2, MPI_INT, &vectorType);
    MPI_Type_commit (&vectorType);

    //Issue the communication, no error should be there
    MPI_Ibcast (&(buf[0]), 1, vectorType, 0 /*root*/, MPI_COMM_WORLD, &(requests[0]));
    MPI_Ibcast (&(buf[1]), 1, vectorType, 0 /*root*/, MPI_COMM_WORLD, &(requests[1]));

    MPI_Waitall (2, requests, statuses);

    //Round 2: Issue the communication, no error should be there
    MPI_Ibcast (&(buf[0]), 1, vectorType, 0 /*root*/, MPI_COMM_WORLD, &(requests[0]));
    MPI_Ibcast (&(buf[1]), 1, vectorType, 0 /*root*/, MPI_COMM_WORLD, &(requests[1]));

    MPI_Waitall (2, requests, statuses);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    //Cleanup user defined datatype
    MPI_Type_free (&vectorType);

    MPI_Finalize ();

    return 0;
}
