/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 4 --must:layout \
// RUN: %builddir/tests/basic/layout.xml --must:analyses \
// RUN: %builddir/tests/basic/analysis_spec.xml %must-bin-dir/basic 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Test log message!.*MPI_Init.*rank}}

/**
 * @file hello.c
 * A must hello world test.
 * Contains no errors.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    printf ("Hello, I am %d of %d tasks.\n", rank, size);

    MPI_Finalize ();

    return 0;
}
