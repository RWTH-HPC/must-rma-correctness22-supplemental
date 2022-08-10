/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 %must-bin-dir/simpleStruct 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file simpleStruct.c
 * A datatype implementation test.
 * Contains no errors.
 *
 * @author Joachim Protze
 */

#include "datatypetest.h"


int main (int argc, char** argv)
{

    MPI_Init(&argc,&argv);

    MPI_Datatype        structtype;

    int blocklens[] = {2, 3, 2, 3};
    MPI_Aint indices[] = {0, 20, 40, 60};
    MPI_Datatype types[] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Type_struct (4, blocklens, indices, types, &structtype);

    // call the real test - this is a macro, that directs to the specific test
    datatypeTest(structtype);

    MPI_Type_free(&structtype);

    MPI_Finalize ();

    return 0;
}
