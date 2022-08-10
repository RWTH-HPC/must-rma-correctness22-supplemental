/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 %must-bin-dir/waitanyDl 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run-ddl %mpiexec-numproc-flag 3 %must-bin-dir/DDlwaitanyDl \
// RUN: 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DDL' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Waitany}}

// CHECK-DDL: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Waitany}}

/**
 * @file waitanyDl.c
 * Simple test with a deadlock involving an MPI_Waitany (Error).
 *
 * Description:
 * Waitany on 1 waits for either 0 or 2, none of them provides something matching.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size,i, index;
    MPI_Status statuses[3];
    MPI_Request requests[3];
    int buf[3];

    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    //Enough tasks ?
    	if (size < 3)
    	{
    		printf ("This test needs at least 3 processes!\n");
    		MPI_Finalize();
    		return 1;
    	}

    	//Say hello
    	printf ("Hello, I am rank %d of %d processes.\n", rank, size);

    	if (rank == 1)
    	{
    		MPI_Irecv (&(buf[0]), 1, MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, &(requests[0]));
    		MPI_Isend (&(buf[1]), 1, MPI_INT, 2, 666, MPI_COMM_WORLD, &(requests[1]));

		MPI_Waitany (2, requests, &index, statuses);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
