/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 4 %must-bin-dir/waitallDl 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run-ddl %mpiexec-numproc-flag 4 %must-bin-dir/DDlwaitallDl \
// RUN: 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DDL' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Waitall}}

// CHECK-DDL: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Waitall}}

/**
 * @file waitallDl.c
 * Deadlock with waitall (Error).
 *
 * Description:
 * A waitall on 1 waits for ranks 0 and 2 who both use wildcard receives.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size;
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

    	if (rank == 0)
    	{
    		MPI_Recv (&(buf[0]), 1, MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, statuses);
    	}

    	if (rank == 1)
    	{
    	    MPI_Isend (&(buf[0]), 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &(requests[0]));
    		MPI_Isend (&(buf[1]), 1, MPI_INT, 2, 123, MPI_COMM_WORLD, &(requests[1]));
    		MPI_Waitall (2, requests, statuses);
    	}

    	if (rank == 2)
    	{
    	    MPI_Recv (&(buf[0]), 1, MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, statuses);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
