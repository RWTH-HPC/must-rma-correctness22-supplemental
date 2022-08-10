/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 %must-bin-dir/simpleIndexedBlock \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file simpleIndexedBlock.c
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

    int indices[] = {0, 4, 10, 14, 20};

    MPI_Type_create_indexed_block (4, 2, indices, MPI_INT, &indexedtype);

    // call the real test - this is a macro, that directs to the specific test
    datatypeTest(indexedtype);

    MPI_Type_free(&indexedtype);

    MPI_Finalize ();

    return 0;
}
