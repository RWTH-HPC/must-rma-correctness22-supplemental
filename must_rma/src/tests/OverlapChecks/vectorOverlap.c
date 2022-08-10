/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 %must-bin-dir/VectorOverlap 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT] Warning: from: call MPI_Type_commit@0: Argument 1 (datatype) is selfoverlapping !
// CHECK: [MUST-REPORT] Error: from: call MPI_Pack@0: Argument 3 (datatype) is selfoverlapping after repetition of 1 !

/**
 * @file vectorOverlap.c
 * A must overlap test.
 * This test contains a vector with shortened extent, that overlaps on 6th repetition (in type contiguous)
 *
 * @author Joachim Protze
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mustTest.h"

int printIntMap(MPI_Datatype type)
{
#ifdef HAVE_MPI_TYPE_GET_TRUE_EXTENT
    int size, int_size, i, pack_size, position=0;
    MPI_Aint true_lb, true_extent;

    MPI_Type_size(type, &size);
    MPI_Type_size(MPI_INT, &int_size);
    MPI_Type_get_true_extent(type, &true_lb, &true_extent);
    MPI_Pack_size(1, type, MPI_COMM_SELF, &pack_size);

    MPI_Datatype serialConti;
    MPI_Type_contiguous(size/int_size, MPI_INT, &serialConti);
    MPI_Type_commit(&serialConti);

    int *inbuf = malloc(true_extent);
    int *outbuf = malloc(size);
    char *packbuff = malloc(pack_size);

    for (i=0; i < true_extent / int_size; i++)
        inbuf[i] = i + true_lb / int_size;
    MPI_Pack(inbuf, 1, type, packbuff, pack_size, &position, MPI_COMM_SELF);
    position=0;
    MPI_Unpack(packbuff, pack_size, &position, outbuf, 1, serialConti, MPI_COMM_SELF);
    for (i=0; i < size/int_size; i++)
        printf("%i, ", outbuf[i]);
    printf("\b\b\n");
    MPI_Fint ftype = MPI_Type_c2f(type);
    MPI_Initialized (&ftype);
    MPI_Type_free(&serialConti);
#endif

    return 0;
}




int main (int argc, char** argv)
{

    int fType;
    MPI_Init(&argc,&argv);

    MPI_Datatype        vectortype, structtype, contitype[5];
    
    MPI_Type_vector (10, 1, 5, MPI_INT, &vectortype);
    
    int                 blocklens[3] = {1,1,1};
    MPI_Aint            displs[3] = {0,0,4};

    MPI_Type_extent(MPI_INT, displs+2);

#ifdef HAVE_MPI_TYPE_CREATE_RESIZED
    MPI_Type_create_resized (vectortype, displs[0], displs[2], &structtype);
#else
    MPI_Datatype        types[3] = {MPI_LB, vectortype, MPI_UB};
    MPI_Type_struct (3, blocklens, displs, types, &structtype);
#endif
    int i;
    for (i=0; i<5; i++)
    {
        MPI_Type_contiguous (i+3, structtype, contitype+i);
        MPI_Type_commit (contitype+i);
        printIntMap(contitype[i]);
    }
    MPI_Type_free(&structtype);
    MPI_Type_free(&vectortype);
    for (i=0; i<5; i++)
    {
        MPI_Type_free (contitype+i);
    }

    MPI_Finalize ();

    return 0;
}
