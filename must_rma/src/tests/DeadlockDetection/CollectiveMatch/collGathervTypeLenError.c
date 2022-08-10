/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/collGathervTypeLenError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collGathervTypeLenErrorlayout.xml \
// RUN: %must-bin-dir/DcollGathervTypeLenError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollGathervTypeLenErrorlayout.xml \
// RUN: %must-bin-dir/DIntracollGathervTypeLenError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-INTRA' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective calls use [(]datatype,count[)] pairs that span type signatures of different length! Each send and receive transfer of a collective call must use equal type signatures.*The first element of the send that did not fit into the receive operation is at [(]MPI_INT[)] in the send type}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length!}}

// CHECK-DISTRIBUTED-INTRA: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length!}}

/**
 * @file collGathervTypeLenError.c
 * A test with an correct MPI_Gatherv call (Error).
 *
 * Description:
 * The root inverts receive order by using the recvdispls.
 * For rank 2 (needs at least 3 tasks) it uses a receive count of 0. (ERROR)
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
    int rank,
         size,
         *temp = NULL,
         *rcounts = NULL,
         *rdispls = NULL,
         i,
         root;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    //Enough tasks ?
    if (size < 3)
    {
        printf ("This test needs at least 3 processes!\n");
        MPI_Finalize();
        return 1;
    }

    //Say hello
    printf ("Hello, I am rank %d of %d processes.\n", rank, size);

    if (rank == 0)
    {
        temp = (int*) malloc (sizeof(int) * size * 2);
        rcounts = (int*) malloc (sizeof(int) * size);
        rdispls = (int*) malloc (sizeof(int) * size);

        for (i = 0; i < size; i++)
        {
            rcounts[i] = 1;
            rdispls[i] = 2*(size - (i+1));
        }
        rcounts[2] = 0; /*THIS IS ERRONEOUS, has to be at least 1*/
    }

    MPI_Gatherv (&rank, 1, MPI_INT, temp, rcounts, rdispls, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        if (temp) free (temp);
        if (rcounts) free (rcounts);
        if (rdispls) free (rdispls);
    }

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
