/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 3 \
// RUN: %must-bin-dir/collBcastRootMismatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-LOCAL' %s

// RUN: %must-run %mpiexec-numproc-flag 3 --must:layout \
// RUN: %builddir/tests/DeadlockDetection/DCollectiveMatch/collBcastRootMismatchErrorlayout.xml \
// RUN: %must-bin-dir/DcollBcastRootMismatchError 2>&1 \
// RUN: | %filecheck --check-prefix 'CHECK-DISTRIBUTED-LAYOUT' %s

// CHECK-LOCAL: [MUST-REPORT]{{.*Two collective operations that use a root process specified conflicting roots}}

// CHECK-DISTRIBUTED-LAYOUT: [MUST-REPORT]{{.*Two collective operations that use a root process specified conflicting roots}}

/**
 * @file collBcastRootMismatchError.cpp
 * Collective matching test.
 *
 * Description:
 * Performs a MPI_Bcast collective with an error
 * ERROR: each rank gives own rank as root (works with OpenMPI!)
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
    
    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Bcast(outbuf, 6/(3-rank), conti, rank, MPI_COMM_WORLD);

    MPI_Type_free(&conti);
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
