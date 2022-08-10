/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/collAlltoallvNoError2 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collAlltoallvNoError2layout.xml \
// RUN: %must-bin-dir/DcollAlltoallvNoError2 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollAlltoallvNoError2layout.xml \
// RUN: %must-bin-dir/DIntracollAlltoallvNoError2 2>&1 \
// RUN: | %filecheck --implicit-check-not \
// RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

/**
 * @file collAlltoallvNoError2.cpp
 * Collective matching test.
 *
 * Description:
 * Performs a MPI_Alltoallv collective with no error
 *
 *
 *  @date 27.03.2012
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

int main (int argc, char** argv)
{
    int size, rank, i;
    int inbuf[200], outbuf[200];

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (size!=3){
        if (rank == 0)
            std::cout << "This test needs 3 processes" << std::endl;
        MPI_Finalize ();
        return 1;
    }
    
    for (i=0; i<100; i++)
        outbuf[i]=rank*100+i;

    MPI_Datatype conti;
    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    int recvcnts[3] = {18,12,6};
    int sendcnts[3] = {18,12,6};
    int rdispls[3] = {0,36,72};
    int sdispls[3] = {0,36,72};
    int sendtypesize;
    sendtypesize = (3-rank);

    MPI_Type_contiguous(sendtypesize, MPI_INT, &conti);
    MPI_Type_commit(&conti);

    for (i=0; i<3; i++)
    {
        recvcnts[i] = ((3 + rank - i)%3+1)*6;
        sendcnts[i] = ((3 - rank + i)%3+1)*6 / sendtypesize;
        sdispls[i] /= sendtypesize;
    }

    MPI_Alltoallv(outbuf, sendcnts, sdispls, conti, inbuf, recvcnts, rdispls, MPI_INT, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
