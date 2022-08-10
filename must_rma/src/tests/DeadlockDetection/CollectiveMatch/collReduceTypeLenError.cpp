/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/collReduceTypeLenError 2>&1 \
// RUN: | %filecheck-may-segfault --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collReduceTypeLenErrorlayout.xml \
// RUN: %must-bin-dir/DcollReduceTypeLenError 2>&1 \
// RUN: | %filecheck-may-segfault --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective calls use [(]datatype,count[)] pairs that span type signatures of different length}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective .* use [(]datatype,count[)] pairs that span type signatures of different length}}

/**
 * @file collReduceTypeLenError.cpp
 * Collective matching test.
 *
 * Description:
 * Performs a MPI_Reduce collective with incompatible datatype lengths
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
    int inbuf[18], outbuf[18];

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (size!=3){
        if (rank == 0)
            std::cout << "This test needs 3 processes" << std::endl;
        MPI_Finalize ();
        return 1;
    }
    
    for (i=0; i<18; i++)
        outbuf[i]=rank*18+i;

    MPI_Datatype conti;
    MPI_Type_contiguous(3-rank,MPI_INT,&conti);
    MPI_Type_commit(&conti);

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Reduce(outbuf, inbuf, 18+rank%2, MPI_INT, MPI_SUM, 1, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
