/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 8 \
// RUN: %must-bin-dir/collScattervNoErrorLarge 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 8 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collScattervNoErrorLargelayout.xml \
// RUN: %must-bin-dir/DcollScattervNoErrorLarge 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 8 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollScattervNoErrorLargelayout.xml \
// RUN: %must-bin-dir/DIntracollScattervNoErrorLarge 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file collScattervNoErrorLarge.cpp
 * Collective matching test.
 *
 * Description:
 * Performs a MPI_Scatterv collective with no error
 *
 *  @date 23.03.2012
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

int main (int argc, char** argv)
{
    int size, rank, i;
    int inbuf[6]; 

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (size<3){
        if (rank == 0)
            std::cout << "This test needs at least 3 processes" << std::endl;
        MPI_Finalize ();
        return 1;
    }
    
    MPI_Datatype conti;
    MPI_Type_contiguous(3-(rank%3),MPI_INT,&conti);
    MPI_Type_commit(&conti);

    int *outbuf, *displs, *sendcnts;
    outbuf=new int[size*10];
    for (i=0; i<size*10; i++)
        outbuf[i]=rank*size*10+i;
    
    displs=new int[size];
    sendcnts=new int[size];
    for (i=0; i<size; i++)
    {
        displs[i]=i*10;
        sendcnts[i]=6;
    }

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Scatterv(outbuf, sendcnts, displs, MPI_INT, inbuf, 6/(3-(rank%3)), conti, 1, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    delete[] outbuf;
    delete[] displs;
    delete[] sendcnts;

    MPI_Finalize ();


    return 0;
}
