/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 1 \
// RUN: %must-bin-dir/NotValidForCommit_MpiDatatypeNull 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error.*call MPI_Type_commit.*Argument 1.*is MPI_DATATYPE_NULL}}

/**
 * @file NotValidForCommit_MPIDatatypeNull.cpp
 * This is a a test for the analysis DatatypeCheck.
 *
 * Description:
 * Performs a MPI_Commit with DatatypeNull, what results in an error
 *
 *
 *  @date 24.05.2011
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>

int main (int argc, char** argv)
{
    int size, rank;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Datatype type1 = MPI_DATATYPE_NULL;

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    if (rank == 0)
    {
        MPI_Type_commit(&type1);
    }
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
