/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 \
// RUN: %must-bin-dir/NotValidForCreate_Undefined 2>&1 \
// RUN: | %filecheck-may-segfault %s

// CHECK: [MUST-REPORT]{{.*Error.*call MPI_Type_contiguous.*Argument 2.*is an unknown datatype}}

/**
 * @file NotValidForCreate_Undefined.cpp
 * This is a a test for the analysis DatatypeCheck.
 *
 * Description:
 * Performs a type creation with an unknown datatype, what results in an error
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

    MPI_Datatype type1, unknown_type, type2;

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    if (rank == 0)
    {
        MPI_Type_contiguous(2, MPI_INT, &type1);
        unknown_type = type1;
        MPI_Type_free(&type1);
        assert(type1 == MPI_DATATYPE_NULL);
        int err = MPI_Type_contiguous(2, unknown_type, &type2);
        std::cout << "Errorcode: " << err << " (MPI_SUCCESS: " << MPI_SUCCESS << ")" << std::endl;
        MPI_Type_free(&type2);
    }
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
