/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 %must-bin-dir/treeRecursion4 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file treeRecursion4.c
 * A datatype implementation test.
 * Contains no errors.
 *
 * @author Joachim Protze
 */

#include "datatypetest.h"


int main (int argc, char** argv)
{

    MPI_Init(&argc,&argv);

    MPI_Datatype        structtype[4];

    int i,j;
    int blocklens[] = {3, 2, 1};
    MPI_Aint indices[] = {12, 24, 32}, extent;
    MPI_Datatype types[] = {MPI_INT, MPI_INT, MPI_INT};

    for (j=0; j<4; j++)
    {
        for (i=1; i<3; i++)
        {
            MPI_Type_extent(types[i-1], &extent);
            indices[i] = indices[i-1] + blocklens[i-1] * extent + 8;
        }
        MPI_Type_struct (3, blocklens, indices, types, &structtype[j]);
        for (i=1; i<3; i++)
        {
            types[i-1] = types[i];
        }
        types[2] = structtype[j];
    }

    // call the real test - this is a macro, that directs to the specific test
    datatypeTest(structtype[3]);

    for (j=0; j<4; j++)
    {
        MPI_Type_free(&structtype[j]);
    }

    MPI_Finalize ();

    return 0;
}
