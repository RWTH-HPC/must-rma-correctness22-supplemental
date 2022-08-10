/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 \
// RUN: %must-bin-dir/SendRecvOverlapGather 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error[:] from[:] call MPI_Gather@.*0[:] The memory regions spanned by the recv part overlaps at the 0[(]th[)] repetition of datatype at its position [(]MPI_INT[)] with regions spanned by the send part of this operation!}}

/**
 * @file overlapGather.c
 * A must overlap test.
 * Contains no errors.
 *
 * @author Joachim Protze
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mustTest.h"

#define COUNT 4


int main (int argc, char** argv)
{

    int fType, pack_size, position=0, i=0, j, rank, size;
    char *packbuff ;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    //Enough tasks ?
    if (size < 2)
    {
        printf("This test needs at least 2 processes!\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Datatype        vectortype, structtype;
    
    MPI_Type_vector (10, 1, 5, MPI_INT, &vectortype);
    
    int                 blocklens[3] = {1,1,1};
    MPI_Aint            displs[3] = {0,0,4};

    MPI_Type_extent(MPI_INT, displs+2);

    int *inbuf = malloc(100000);
#ifdef HAVE_MPI_TYPE_CREATE_RESIZED
    MPI_Type_create_resized (vectortype, displs[0], displs[2], &structtype);
#else
    MPI_Datatype        types[3] = {MPI_LB, vectortype, MPI_UB};
    MPI_Type_struct (3, blocklens, displs, types, &structtype);
#endif
    MPI_Type_commit(&structtype);

    //Say hello
    printf("Hello, I am rank %i of %i processes.\n", rank, size);;

    MPI_Gather(inbuf, 1, MPI_INT, inbuf, 1, MPI_INT, 0, MPI_COMM_WORLD);

//     MPI_Startall(COUNT,request);
    MPI_Type_free(&vectortype);
    MPI_Type_free(&structtype);
    free(inbuf);

    MPI_Finalize ();

    return 0;
}
