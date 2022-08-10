/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/MatchContiMissmatchErrorlayout.xml \
// RUN: %must-bin-dir/MatchContiMissmatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
// RUN: %builddir/tests/TypeMatch/DMatchContiMissmatchErrorlayout.xml \
// RUN: %must-bin-dir/DMatchContiMissmatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*A send and a receive operation use datatypes that do not match!}}

// CHECK-DISTRIBUTED: [MUST-REPORT]{{.*A send and a receive operation use datatypes that do not match!}}

/**
 * @file MatchContiMissmatchError.c
 * Type matching test with an error.
 *
 * Description:
 * A single send-recv match, the send uses a contiguous type the receive not, the spanned typesignatues
 * do not match (MPI_CHAR != MPI_UNSIGNED_CHAR) (Error).
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
    char data[10];
    unsigned char udata[10];
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
    	MPI_Type_contiguous (10, MPI_CHAR, &conti);
    	MPI_Type_commit (&conti);

    	if (rank == 0)
    		MPI_Send (data, 1, conti, 1, 666, MPI_COMM_WORLD);

    	if (rank == 1)
    		MPI_Recv (udata, 10, MPI_UNSIGNED_CHAR, 0, 666, MPI_COMM_WORLD, &status);

    	MPI_Type_free(&conti);
    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
