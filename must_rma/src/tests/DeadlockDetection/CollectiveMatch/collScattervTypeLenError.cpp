/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/collScattervTypeLenError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collScattervTypeLenErrorlayout.xml \
// RUN: %must-bin-dir/DcollScattervTypeLenError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollScattervTypeLenErrorlayout.xml \
// RUN: %must-bin-dir/DIntracollScattervTypeLenError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-INTRA' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective calls use [(]datatype,count[)] pairs that span type signatures of different length}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length}}

// CHECK-DISTRIBUTED-INTRA: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length}}

/**
 * @file collScattervTypeLenError.cpp
 * This is a test for the analysis CollAgg.
 *
 * Description:
 * Performs a MPI_Scatterv collective with an error
 * ERROR: send {6,6,5} INTs, recv {6,6,6} INTs (works with OpenMPI!)
 *
 *  @date 23.03.2012
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

int main (int argc, char** argv)
{
    int size, rank, i;
    int inbuf[6], outbuf[100];

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (size!=3){
        if (rank == 0)
            std::cout << "This test needs 3 processes" << std::endl;
        MPI_Finalize ();
        return 1;
    }
    
    for (i=0; i<6; i++)
        outbuf[i]=rank*6+i;

    MPI_Datatype conti;
    MPI_Type_contiguous(3-rank,MPI_INT,&conti);
    MPI_Type_commit(&conti);
    
    int displs[3] = {0,10,20};
    int sendcnts[3] = {6,6,5};

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Scatterv(outbuf, sendcnts, displs, MPI_INT, inbuf, 6/(3-rank), conti, 1, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
