/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 %must-bin-dir/AnnotationWarningIfZero \
// RUN: 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Warning: from: call Annotate.*}}

/**
 * @file AnnotationWarningIfZero.cpp
 * This is a test for the user annotation interface.
 *
 *  @date 18.03.2022
 *  @author Felix Tomski
 */

#include <iostream>
#include <mpi.h>
#include <GTI_Annotations.h>

int main (int argc, char** argv)
{
	int buf_size = 0;
	MPI_Init (&argc, &argv);

    GTI_AnnotateIntegerWarningIfZero (buf_size);

	MPI_Finalize ();

	return 0;
}
