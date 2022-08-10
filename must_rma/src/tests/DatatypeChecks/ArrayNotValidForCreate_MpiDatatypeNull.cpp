/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// RUN: %must-run %mpiexec-numproc-flag 1 \
// RUN: %must-bin-dir/ArrayNotValidForCreate_MpiDatatypeNull 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT] Error: from: call MPI_Type_create_struct@0: Element of Array-Argument 4 (array_of_types[0]) is MPI_DATATYPE_NULL!

/**
 * @file ArrayNotValidForCreate_MPIDatatypeNull.cpp
 * This is a a test for the analysis DatatypeCheck.
 *
 * Description:
 * Performs a struct creation with DatatypeNull, what results in an error
 *
 *
 *  @date 23.05.2011
 *  @author Joachim Protze
 */

#include <iostream>
#include <mpi.h>
#include "mustTest.h"

int main (int argc, char** argv)
{
    int size, rank;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Datatype oldtypes[] = {MPI_DATATYPE_NULL,MPI_DATATYPE_NULL,MPI_DATATYPE_NULL,MPI_DATATYPE_NULL,MPI_DATATYPE_NULL}, type2;
    MPI_Aint displs[] =  {10,20,30,40,50};
    int blocklens[] = {1,1,1,1,1};

    //Say hello
    std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;

    if (rank == 0)
    {
        MPI_Type_struct(5, blocklens, displs, oldtypes, &type2);
        MPI_Type_free(&type2);
    }
    std::cout << "Signing off, rank " << rank << "." << std::endl;

    MPI_Finalize ();

    return 0;
}
