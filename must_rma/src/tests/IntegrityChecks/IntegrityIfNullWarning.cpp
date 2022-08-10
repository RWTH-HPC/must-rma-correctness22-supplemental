/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// XFAIL: *
// RUN: %must-run %mpiexec-numproc-flag 1 \
// RUN: %must-bin-dir/IntegrityIfNullWarning 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Warning: from: call MPI_Keyval_create@.*: Argument 4 [(]extra_state[)] is a NULL pointer,  which is allowed but unusual}}

/**
 * @file IntegrityIfNullWarning.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 * Description:
 * Creates a keyval using keyval_create and a extra_state of null, what will cause a warning.
 *
 *  @date 30.05.2011
 *  @author Mathias Korepkat
 */

#include <iostream>
#include <mpi.h>
#include "mustTest.h"

int main (int argc, char** argv)
{
	int size, rank;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	MPI_Status status;

	//Enough tasks ?
	if (size < 1)
	{
		std::cerr << "This test needs at least 1 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;


	int key=1;

	//MPI_Buffer_attach( buf, bufSize );
	MPI_Keyval_create (MPI_NULL_COPY_FN, MPI_NULL_DELETE_FN, &key, NULL/*will cause a warning */);
	MPI_Keyval_free(&key);
	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
