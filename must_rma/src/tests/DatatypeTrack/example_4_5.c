/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 --must:layout \
// RUN: %builddir/tests/DatatypeTrack/Example_4_5Layout.xml --must:analyses \
// RUN: %builddir/tests/DatatypeTrack/analysis_spec.xml \
// RUN: %must-bin-dir/Example_4_5 2>&1 \
// RUN: | %filecheck %s

/**
 * @file example_4_5.c
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

    MPI_Datatype        oldtype, newtype;
    int                 blocklens[2] = {1,1};
    MPI_Aint            displs[2] = {0,8};
    int                 indexedDispls[2] = {4,0};
    MPI_Datatype        types[2] = {MPI_DOUBLE, MPI_CHAR};

    MPI_Type_struct (2, blocklens, displs, types, &oldtype);
    // CHECK-DAG: Typemap = {(MPI_DOUBLE, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}})}
    test_type (oldtype);

    blocklens[0] = 3;
    blocklens[1] = 1;

    MPI_Type_indexed (2, blocklens, indexedDispls, oldtype, &newtype);
    // CHECK-DAG: Typemap = {(MPI_DOUBLE, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}}), (MPI_DOUBLE, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}}), (MPI_DOUBLE, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}}), (MPI_DOUBLE, {{[0-9]+}}), (MPI_CHAR, {{[0-9]+}})}
    test_type (newtype);

    MPI_Finalize ();

    return 0;
}
