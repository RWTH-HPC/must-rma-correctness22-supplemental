/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// REQUIRES: HAVE_MPI_REDUCE_SCATTER_BLOCK
// RUN: %must-run %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/collReduceScatterBlockNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collReduceScatterBlockNoErrorlayout.xml \
// RUN: %must-bin-dir/DcollReduceScatterBlockNoError 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file collReduceScatterBlockNoError.cpp
 * Collective matching test.
 *
 * Description:
 * Performs a MPI_Reduce_scatter_block collective with no error
 *
 *  @date 27.03.2012
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

int main (int argc, char** argv)
{
    int size, rank, i;
    int inbuf[20], outbuf[20];

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (size!=3){
        if (rank == 0)
            std::cout << "This test needs 3 processes" << std::endl;
        MPI_Finalize ();
        return 1;
    }
    
    for (i=0; i<20; i++)
        outbuf[i]=rank*20+i;

    MPI_Datatype conti;
    MPI_Type_contiguous(3-rank,MPI_INT,&conti);
    MPI_Type_commit(&conti);

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Reduce_scatter_block(outbuf, inbuf, 6, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
