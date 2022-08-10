/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 --must:layout \
// RUN: %builddir/tests/DatatypeTrack/Example_4_6Layout.xml --must:analyses \
// RUN: %builddir/tests/DatatypeTrack/analysis_spec.xml \
// RUN: %must-bin-dir/Example_4_6 2>&1 \
// RUN: | %filecheck %s

/**
 * @file example_4_6.c
 * A must datatype test.
 * Contains no errors.
 *
 * @author Joachim Protze
 */

#include "datatypeTest.h"

int main (int argc, char** argv)
{

    int fType;
    MPI_Init(&argc,&argv);

    MPI_Datatype        type1, newtype;
    int                 blocklens[3] = {1,1,3};
    MPI_Aint            displs[3] = {0,8,26};
    MPI_Datatype        types[3] = {MPI_DOUBLE, MPI_CHAR, MPI_CHAR};

    MPI_Type_struct (2, blocklens, displs, types, &type1);
    // CHECK-DAG: Typemap = {(MPI_DOUBLE, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}})}
    test_type (type1);


    blocklens[0] = 2;
    blocklens[1] = 1;
    displs[0] = 0;
    displs[1] = 16;
    types[0] = MPI_FLOAT;
    types[1] = type1;
    MPI_Type_struct (3, blocklens, displs, types, &newtype);
    // CHECK-DAG: Typemap = {(MPI_FLOAT, {{[0-9]+}}), (MPI_FLOAT, {{[0-9]+}}), (MPI_DOUBLE, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}})}
    test_type (newtype);

    MPI_Finalize ();

    return 0;
}
