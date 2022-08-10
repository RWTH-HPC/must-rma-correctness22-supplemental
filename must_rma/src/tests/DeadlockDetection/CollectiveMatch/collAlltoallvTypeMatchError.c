/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 5 \
// RUN: %must-bin-dir/collAlltoallvTypeMatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 5 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collAlltoallvTypeMatchErrorlayout.xml \
// RUN: %must-bin-dir/DcollAlltoallvTypeMatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// RUN: %must-run %mpiexec-numproc-flag 5 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollAlltoallvTypeMatchErrorlayout.xml \
// RUN: %must-bin-dir/DIntracollAlltoallvTypeMatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-INTRA' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective calls cause a type mismatch!.*The mismatch occurs at [(]struct[)].*[(]MPI_INT[)] in the send type and at [(]struct[)].*[(]MPI_UNSIGNED[)] in the receive type}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective calls cause a type mismatch!.*occurs at [(]struct[)].*}}

// CHECK-DISTRIBUTED-INTRA: [MUST-REPORT]{{.*Two collective calls cause a type mismatch!.*occurs at [(]struct[)].*}}

/**
 * @file collAlltoallvTypeMatchError.c
 * A test with an incorrect MPI_Alltoallv call (Error).
 *
 * Description:
 * All processes execute an MPI_Alltoallv with matching and valid arguments,
 * except for rank 3 which uses an incorrect recv type.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mustTest.h"

int main (int argc, char** argv)
{
    //Basic and all2allv stuff
    int rank,
         size,
         *sbuf = NULL,
         *rbuf = NULL,
         *scounts = NULL,
         *rcounts = NULL,
         *sdispls = NULL,
         *rdispls = NULL,
         i,
         root;
    MPI_Status status;

    //Recv type stuff
    MPI_Datatype        srtype, misstype, stype, rtype;
    int                        blocklens[2] = {1,1};
    MPI_Aint               displs[2] = {0,sizeof(int)};
    MPI_Datatype        oldtypes[2] = {MPI_INT, MPI_INT};

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

    //SendRecv type
    MPI_Type_struct (2, blocklens, displs, oldtypes, &srtype);
    MPI_Type_commit (&srtype);

    //Type for mismatch
    oldtypes[1] = MPI_UNSIGNED;
    MPI_Type_struct (2, blocklens, displs, oldtypes, &misstype);
    MPI_Type_commit (&misstype);

    //Preparation of all2allv
    sbuf = (int*) malloc (sizeof(int) * size*2);
    rbuf = (int*) malloc (sizeof(int) * size*2);
    scounts = (int*) malloc (sizeof(int) * size);
    rcounts = (int*) malloc (sizeof(int) * size);
    sdispls = (int*) malloc (sizeof(int) * size);
    rdispls = (int*) malloc (sizeof(int) * size);

    for (i = 0; i < size; i++)
    {
        scounts[i] = 1;
        rcounts[i] = 1;
        sdispls[i] = (size - (i+1));
        rdispls[i] = i;
    }

    stype = rtype = srtype;

    if (rank == 3) /*ERROR: rank 3 uses wrong receive type*/
        rtype=misstype;

    MPI_Alltoallv (sbuf, scounts, sdispls, stype, rbuf, rcounts, rdispls, rtype, MPI_COMM_WORLD);

    //Say bye bye
    printf ("Signing off, rank %d.\n", rank);

    if (sbuf) free (sbuf);
    if (rbuf) free (rbuf);
    if (scounts) free (scounts);
    if (rcounts) free (rcounts);
    if (sdispls) free (sdispls);
    if (rdispls) free (rdispls);

    MPI_Type_free (&srtype);
    MPI_Type_free (&misstype);

    MPI_Finalize ();

    return 0;
}
