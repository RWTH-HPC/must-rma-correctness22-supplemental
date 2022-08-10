/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 %must-bin-dir/simpleSubarray 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file simpleSubarray.c
 * A datatype implementation test.
 * Contains no errors.
 *
 * @author Joachim Protze
 */

#include "datatypetest.h"


int main (int argc, char** argv)
{

    MPI_Init(&argc,&argv);

    MPI_Datatype        subarraytype;

    int sizes[] = {5,5,5};
    int subsizes[] = {2,2,2};
    int starts[] = {1,2,3};

    MPI_Type_create_subarray (3, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &subarraytype);

    // call the real test - this is a macro, that directs to the specific test
    datatypeTest(subarraytype);

    MPI_Type_free(&subarraytype);

    MPI_Finalize ();

    return 0;
}
