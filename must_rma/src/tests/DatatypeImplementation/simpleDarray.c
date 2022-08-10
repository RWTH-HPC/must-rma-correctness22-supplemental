/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 %must-bin-dir/simpleDarray 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file simpleDarray.c
 * A datatype implementation test.
 * Contains no errors.
 *
 * @author Joachim Protze
 */

#include "datatypetest.h"


int main (int argc, char** argv)
{

    MPI_Init(&argc,&argv);

    MPI_Datatype        darraytype;

    int gsizes[] = {6, 6, 2};
    int distribs[] = {MPI_DISTRIBUTE_CYCLIC, MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_NONE};
    int dargs[] = {1, MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG};
    int psizes[] = {2, 2, 1};

    MPI_Type_create_darray (4, 2, 3, gsizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_INT, &darraytype);

    // call the real test - this is a macro, that directs to the specific test
    datatypeTest(darraytype);

    MPI_Type_free(&darraytype);

    MPI_Finalize ();

    return 0;
}
