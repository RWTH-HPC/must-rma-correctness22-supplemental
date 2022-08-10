/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/recvDl 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run-ddl %mpiexec-numproc-flag 3 %must-bin-dir/DDlrecvDl 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DDL' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Recv}}

// CHECK-DDL: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Recv}}

/**
 * @file recvDl.c
 * Simple deadlock caused by two receive calls.
 *
 * Description:
 * Actual deadlock (ERROR).
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size,buf1, buf2;
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
    	{
    	    MPI_Recv (&buf2, 1, MPI_INT, 1, 666, MPI_COMM_WORLD, &status);
    		MPI_Send (&buf1, 1, MPI_INT, 1, 666, MPI_COMM_WORLD);
    	}

    	if (rank == 1)
    	{
    	    MPI_Recv (&buf2, 1, MPI_INT, 0, 666, MPI_COMM_WORLD, &status);
    	    MPI_Send (&buf1, 1, MPI_INT, 0, 666, MPI_COMM_WORLD);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
