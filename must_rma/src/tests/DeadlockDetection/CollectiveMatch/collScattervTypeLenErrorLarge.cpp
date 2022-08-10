/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 8 \
// RUN: %must-bin-dir/collScattervTypeLenErrorLarge 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 8 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collScattervTypeLenErrorLargelayout.xml \
// RUN: %must-bin-dir/DcollScattervTypeLenErrorLarge 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// RUN: %must-run %mpiexec-numproc-flag 8 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/DIntracollScattervTypeLenErrorLargelayout.xml \
// RUN: %must-bin-dir/DIntracollScattervTypeLenErrorLarge 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-INTRA' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective calls use [(]datatype,count[)] pairs that span type signatures of different length}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length}}

// CHECK-DISTRIBUTED-INTRA: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length}}

/**
 * @file collScattervTypeLenErrorLarge.cpp
 * Collective matching test.
 *
 * Description:
 * Performs a MPI_Scatterv collective with type matching errors (signature lengths are not right) (Error).
 *
 *
 *  @date 23.03.2012
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

#define ROOT 1

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

    if (rank == ROOT)
        MPI_Scatterv(outbuf, sendcnts, displs, MPI_INT, inbuf, 6/(3-(rank%3)), conti, ROOT, MPI_COMM_WORLD);
    else if (rank == size-1)
        MPI_Scatterv(NULL, NULL, NULL, MPI_DATATYPE_NULL, inbuf, 6/(3-(rank%3))+1, conti, ROOT, MPI_COMM_WORLD);
    else
        MPI_Scatterv(NULL, NULL, NULL, MPI_DATATYPE_NULL, inbuf, 6/(3-(rank%3)), conti, ROOT, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    delete[] outbuf;
    delete[] displs;
    delete[] sendcnts;

    MPI_Finalize ();


    return 0;
}
