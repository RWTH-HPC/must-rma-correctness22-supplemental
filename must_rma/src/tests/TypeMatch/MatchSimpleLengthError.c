/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/MatchSimpleLengthErrorlayout.xml \
// RUN: %must-bin-dir/MatchSimpleLengthError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/DMatchSimpleLengthErrorlayout.xml \
// RUN: %must-bin-dir/DMatchSimpleLengthError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*A receive operation uses a [(]datatype,count[)] pair that can not hold the data transfered by the send it matches}}

// CHECK-DISTRIBUTED: [MUST-REPORT]{{.*A receive operation uses a [(]datatype,count[)] pair that can not hold the data transfered by the send it matches}}

/**
 * @file MatchSimpleLengthError.c
 * Simple send recv test with a truncated receive.
 *
 * Description:
 * There is a single send/recv pair where the receive count it too smal and a truncation occurs, which is an error.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
    int data[2];
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
    		MPI_Send (data, 2 /*ERROR here: too long (or recv to short)*/, MPI_INT, 1, 666, MPI_COMM_WORLD);

    	if (rank == 1)
    		MPI_Recv (data, 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &status);

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
