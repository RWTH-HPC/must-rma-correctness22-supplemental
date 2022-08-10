/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/MatchSimpleTypeMissmatchErrorlayout.xml \
// RUN: %must-bin-dir/MatchSimpleTypeMissmatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/DMatchSimpleTypeMissmatchErrorlayout.xml \
// RUN: %must-bin-dir/DMatchSimpleTypeMissmatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*A send and a receive operation use datatypes that do not match!}}

// CHECK-DISTRIBUTED: [MUST-REPORT]{{.*A send and a receive operation use datatypes that do not match!}}

/**
 * @file MatchSimpleTypeMissmatchError.c
 * Simple send recv test with a type missmatch bewteen a single MPI_CHAR and a single MPI_UNSIGNED_CHAR.
 *
 * Description:
 * Type missmatch (Error).
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
    char data[1];
    unsigned char udata[1];
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    //Enough tasks ?
    	if (size < 2)
    	{
    		printf ("This test needs at least 2 processes!\n");
    		MPI_Finalize();
    		return 1;
    	}

    	//Say hello
    	printf ("Hello, I am rank %d of %d processes.\n", rank, size);

    	if (rank == 0)
    		MPI_Send (data, 1, MPI_CHAR /*ERROR HERE, MPI_CHAR != MPI_UNSIGNED_CHAR*/, 1, 666, MPI_COMM_WORLD);

    	if (rank == 1)
    		MPI_Recv (udata, 1, MPI_UNSIGNED_CHAR , 0, 666, MPI_COMM_WORLD, &status);

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
