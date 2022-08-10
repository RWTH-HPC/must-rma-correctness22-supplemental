/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/collGathervTypeLenError2 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collGathervTypeLenError2layout.xml \
// RUN: %must-bin-dir/DcollGathervTypeLenError2 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollGathervTypeLenError2layout.xml \
// RUN: %must-bin-dir/DIntracollGathervTypeLenError2 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-INTRA' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective calls use [(]datatype,count[)] pairs that span type signatures of different length}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length}}

// CHECK-DISTRIBUTED-INTRA: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length}}

/**
 * @file collGathervTypeLenError2.cpp
 * Collective matching test.
 *
 * Description:
 * Performs a MPI_Gatherv collective with an error
 * ERROR: send {6,6,6} INTs, recv {6,6,7} INTs (works with OpenMPI!)
 *
 *  @date 23.03.2012
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

int main (int argc, char** argv)
{
    int size, rank, i;
    int inbuf[100], outbuf[6];

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
    MPI_Type_contiguous(rank+1,MPI_INT,&conti);
    MPI_Type_commit(&conti);
    
    int displs[3] = {0,10,20};
    int recvcnts[3] = {6,6,7};

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Gatherv(outbuf, 6/(rank+1), conti, inbuf, recvcnts, displs, MPI_INT, 1, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
