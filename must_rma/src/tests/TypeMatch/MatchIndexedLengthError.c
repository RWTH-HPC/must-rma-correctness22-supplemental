/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/MatchIndexedLengthErrorlayout.xml \
// RUN: %must-bin-dir/MatchIndexedLengthError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/DMatchIndexedLengthErrorlayout.xml \
// RUN: %must-bin-dir/DMatchIndexedLengthError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*A receive operation uses a [(]datatype,count[)] pair that can not hold the data transfered by the send it matches}}

// CHECK-DISTRIBUTED: [MUST-REPORT]{{.*A receive operation uses a [(]datatype,count[)] pair that can not hold the data transfered by the send it matches}}

/**
 * @file MatchIndexedLengthError.c
 * Type matching test with an error.
 *
 * Description:
 * A single send-recv match, the send and reveice both use indexed type,
 * the sender sends too much data (Error).
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
    long data[100];
    MPI_Status status;
    MPI_Datatype newType;
    int blocklengths[3] = {4,3,2};
    int displacements[3]= {4, 8, 12};
    int count = 3;

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

    	//Create a conti type
    	if (rank == 1)
    	    blocklengths[2] = 1; //ERROR must be equal on both tasks
    	MPI_Type_indexed (count, blocklengths, displacements, MPI_LONG, &newType);
    	MPI_Type_commit (&newType);

    	if (rank == 0)
    		MPI_Send (data, 2, newType, 1, 666, MPI_COMM_WORLD);

    	if (rank == 1)
    		MPI_Recv (data, 2, newType, 0, 666, MPI_COMM_WORLD, &status);

    	MPI_Type_free(&newType);
    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
