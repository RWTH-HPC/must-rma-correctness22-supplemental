/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 1 \
// RUN: %must-bin-dir/NotValidForCommunication_NotCommited 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error.*call MPI_Bcast.*Argument 3.*is not commited for transfer}}

/**
 * @file NotValidForCommunication_NotCommited.cpp
 * This is a test for the analysis DatatypeCheck.
 *
 * Description:
 * Performs a MPI_Bcast with an uncommited datatype what results in an error
 *
 *
 *  @date 24.05.2011
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>
#include <assert.h>

int main (int argc, char** argv)
{
    int size, rank;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Datatype type1;

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    MPI_Type_contiguous(1,MPI_INT,&type1);
    int err = MPI_Bcast(&size, 1, type1, 0, MPI_COMM_WORLD);
    std::cout << "Errorcode: " << err << " (MPI_SUCCESS: " << MPI_SUCCESS << ")" << std::endl;
    MPI_Type_free(&type1);

    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
