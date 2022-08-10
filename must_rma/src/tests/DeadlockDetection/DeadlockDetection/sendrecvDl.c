/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 4 %must-bin-dir/sendrecvDl 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run-ddl %mpiexec-numproc-flag 4 %must-bin-dir/DDlsendrecvDl \
// RUN: 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DDL' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Sendrecv}}

// CHECK-DDL: [MUST-REPORT]{{.*The application issued a set of MPI calls that can cause a deadlock!.*MPI_Sendrecv}}

/**
 * @file sendrecvDl.c
 * Simple sendrecv deadlock. (Error)
 *
 * Description:
 * A sendrecv deadlocks here, it uses a wildcard for the receive part.
 *
 * @author Tobias Hilbrich
 */

#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    int rank,size, sbuf, rbuf;
    MPI_Status status;

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
    		MPI_Send (&sbuf, 1, MPI_INT, 1, 123, MPI_COMM_WORLD);

    	if (rank == 1)
    		MPI_Sendrecv (&sbuf, 1, MPI_INT, 2, 123, &rbuf, 1, MPI_INT, MPI_ANY_SOURCE, 789, MPI_COMM_WORLD, &status); /*ERROR: recv tag is wrong*/

    	if (rank == 2)
    	{
    	    MPI_Recv (&rbuf, 1, MPI_INT, 1, 123, MPI_COMM_WORLD, &status);
    	    MPI_Send (&sbuf, 1, MPI_INT, 1, 123, MPI_COMM_WORLD);
    	}

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
