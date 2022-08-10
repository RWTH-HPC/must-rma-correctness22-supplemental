/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 1 \
// RUN: %must-bin-dir/NotValidForCreate_MpiDatatypeNull 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*Error.*call MPI_Type_contiguous.*Argument 2.*is MPI_DATATYPE_NULL}}

/**
 * @file NotValidForCreate_MPIDatatypeNull.cpp
 * This is a a test for the analysis DatatypeCheck.
 *
 * Description:
 * Performs a type creation with DatatypeNull, what results in an error
 *
 *
 *  @date 23.05.2011
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

    MPI_Datatype type1 = MPI_DATATYPE_NULL, type2;

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    if (rank == 0)
    {
        MPI_Type_contiguous(2, type1, &type2);
        MPI_Type_free(&type2);
    }
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
