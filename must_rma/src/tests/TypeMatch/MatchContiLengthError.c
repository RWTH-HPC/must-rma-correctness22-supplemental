/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/MatchContiLengthErrorlayout.xml \
// RUN: %must-bin-dir/MatchContiLengthError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/DMatchContiLengthErrorlayout.xml \
// RUN: %must-bin-dir/DMatchContiLengthError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*A receive operation uses a [(]datatype,count[)] pair that can not hold the data transfered by the send it matches}}

// CHECK-DISTRIBUTED: [MUST-REPORT]{{.*A receive operation uses a [(]datatype,count[)] pair that can not hold the data transfered by the send it matches}}

/**
 * @file MatchContiLengthError.c
 * Type matching test with an error.
 *
 * Description:
 * A single send-recv match, the send uses a contiguous type the receive not, the spanned typesignatues
 * do not match as the send is type too long (Error).
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
    long data[11];
    MPI_Status status;
    MPI_Datatype conti;

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
    	MPI_Type_contiguous (11, MPI_LONG, &conti);
    	MPI_Type_commit (&conti);

    	if (rank == 0)
    		MPI_Send (data, 1, conti, 1, 666, MPI_COMM_WORLD);

    	if (rank == 1)
    		MPI_Recv (data, 10 /*ERROR: too short must be 11 at least*/, MPI_LONG, 0, 666, MPI_COMM_WORLD, &status);

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
