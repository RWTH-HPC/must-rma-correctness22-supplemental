/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 8 \
// RUN: %must-bin-dir/collGathervNoErrorLarge 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 8 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collGathervNoErrorLargelayout.xml \
// RUN: %must-bin-dir/DcollGathervNoErrorLarge 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 8 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollGathervNoErrorLargelayout.xml \
// RUN: %must-bin-dir/DIntracollGathervNoErrorLarge 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file GatherVSuccess.cpp
 * Collective matching test.
 *
 * Description:
 * Performs a MPI_Gatherv collective with no error
 *
 *
 *  @date 23.03.2012
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

int main (int argc, char** argv)
{
    int size, rank, i;
    int outbuf[6];

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (size<3){
        if (rank == 0)
            std::cout << "This test needs at least 3 processes" << std::endl;
        MPI_Finalize ();
        return 1;
    }
    
    for (i=0; i<6; i++)
        outbuf[i]=rank*6+i;

    MPI_Datatype conti;
    MPI_Type_contiguous(3-(rank%3),MPI_INT,&conti);
    MPI_Type_commit(&conti);
    
    int *inbuf, *displs, *recvcnts;
    inbuf=new int[size*10];
    
    displs=new int[size];
    recvcnts=new int[size];
    for (i=0; i<size; i++)
    {
        displs[i]=i*10;
        recvcnts[i]=6;
    }

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Gatherv(outbuf, 6/(3-(rank%3)), conti, inbuf, recvcnts, displs, MPI_INT, 1, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    delete[] inbuf;
    delete[] displs;
    delete[] recvcnts;

    MPI_Finalize ();

    return 0;
}
