/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 %must-bin-dir/simpleIndexed 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file simpleIndexed.c
 * A datatype implementation test.
 * Contains no errors.
 *
 * @author Joachim Protze
 */

#include "datatypetest.h"


int main (int argc, char** argv)
{

    MPI_Init(&argc,&argv);

    MPI_Datatype        indexedtype;

    int blocklens[] = {2, 3, 2, 3};
    int indices[] = {3, 7, 11, 15};

    MPI_Type_indexed (4, blocklens, indices, MPI_INT, &indexedtype);

    // call the real test - this is a macro, that directs to the specific test
    datatypeTest(indexedtype);

    MPI_Type_free(&indexedtype);

    MPI_Finalize ();

    return 0;
}
