/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file mpi1Datatypetest.c
 * A datatype implementation test.
 * Contains no errors.
 *
 * @author Joachim Protze
 */

#include "datatypetest.h"


int main (int argc, char** argv)
{

    int fType;
    MPI_Init(&argc,&argv);

    MPI_Datatype        vectortype, structtype, contitype;
    
    MPI_Type_vector (10, 1, 5, MPI_INT, &vectortype);
    
    int                 blocklens[3] = {1,1,1};
    MPI_Aint            displs[3] = {0,0,4};
    MPI_Datatype        types[3] = {MPI_LB, vectortype, MPI_UB};

    MPI_Type_extent(MPI_INT, displs+2);

    MPI_Type_struct (3, blocklens, displs, types, &structtype);

    // call the real test - this is a macro, that directs to the specific test
    datatypeTest(structtype);

    MPI_Type_free(&vectortype);
    MPI_Type_free(&structtype);

    MPI_Finalize ();

    return 0;
}
