/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// XFAIL: *
// RUN: %must-run %mpiexec-numproc-flag 4 %must-bin-dir/waitAnySourceEx4Dl \
// RUN: 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run-ddl %mpiexec-numproc-flag 4 \
// RUN: %must-bin-dir/DDlwaitAnySourceEx4Dl 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DDL' %s

// CHECK-LOCAL: [MUST-REPORT] Information global: MUST issued a deadlock detection while a wildcard receive call (MPI_Recv/MPI_Irecv with MPI_ANY_SOURCE) was not yet completed.
// CHECK-LOCAL: [MUST-REPORT] Error global: The application issued a set of MPI calls that can cause a deadlock!{{.*MPI_Recv}}

// CHECK-DDL: [MUST-REPORT] Information global: MUST issued a deadlock detection while a wildcard receive call (MPI_Recv/MPI_Irecv with MPI_ANY_SOURCE) was not yet completed.
// CHECK-DDL: [MUST-REPORT] Error global: The application issued a set of MPI calls that can cause a deadlock!{{.*MPI_Recv}}

/**
 * @file waitAnySourceEx4Dl.c
 * Complex example where a deadlock happens while wildcard source updates won't arrive, even though they are partners available for them.
 *
 * Description:
 * Complex deadlock, gives MUST a hard time.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size, buf;
    MPI_Status status;
    MPI_Request request;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    //Enough tasks ?
        if (size < 4)
        {
            printf ("This test needs at least 4 processes!\n");
            MPI_Finalize();
            return 1;
        }

        //Say hello
        printf ("Hello, I am rank %d of %d processes.\n", rank, size);

        if (rank == 0)
        {
            MPI_Irecv (&size, 1, MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, &request);
            //This is the error, would also have to provide a send

            MPI_Recv (&buf, 1, MPI_INT, 3, 123, MPI_COMM_WORLD, &status); //This will hang

            MPI_Wait (&request, &status);
        }
        if (rank == 1)
        {
            MPI_Irecv (&size, 1, MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, &request);
            MPI_Send (&buf, 1, MPI_INT, 0, 666, MPI_COMM_WORLD);

            MPI_Recv (&buf, 1, MPI_INT, 3, 123, MPI_COMM_WORLD, &status); //This will hang

            MPI_Wait (&request, &status);
        }
        if (rank == 2)
        {
            MPI_Irecv (&size, 1, MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, &request);
            MPI_Send (&buf, 1, MPI_INT, 1, 666, MPI_COMM_WORLD);

            MPI_Recv (&buf, 1, MPI_INT, 3, 123, MPI_COMM_WORLD, &status); //This will hang

            MPI_Wait (&request, &status);
        }
        if (rank == 3)
        {
            MPI_Irecv (&size, 1, MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, &request);
            MPI_Send (&buf, 1, MPI_INT, 2, 666, MPI_COMM_WORLD);
            MPI_Wait (&request, &status); //This will hang

            MPI_Send (&buf, 1, MPI_INT, 0, 123, MPI_COMM_WORLD);
            MPI_Send (&buf, 1, MPI_INT, 1, 123, MPI_COMM_WORLD);
            MPI_Send (&buf, 1, MPI_INT, 2, 123, MPI_COMM_WORLD);
        }

        //Say bye bye
        printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
