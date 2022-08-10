/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 %must-bin-dir/sendrecvProcNullNoDl \
// RUN: 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run-ddl %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/DDlsendrecvProcNullNoDl 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file sendrecvProcNullNoDl.c
 * Simple sendrecv test for deadlock detection, complicated with MPI_PROC_NULL. (No Error)
 *
 * Description:
 * There is no deadlock in this test, we call a matching transfers.
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

    	if (rank == 1)
    		MPI_Sendrecv (&sbuf, 1, MPI_INT, 2, 123, &rbuf, 1, MPI_INT, MPI_PROC_NULL, 666, MPI_COMM_WORLD, &status);

    	if (rank == 2)
    	    MPI_Recv (&rbuf, 1, MPI_INT, 1, 123, MPI_COMM_WORLD, &status);

    	//Say bye bye
    	printf ("Signing off, rank %d.\n", rank);

    MPI_Finalize ();

    return 0;
}
