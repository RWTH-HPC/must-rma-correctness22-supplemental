/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 --must:layout \
// RUN: %builddir/tests/DatatypeTrack/Example_4_2Layout.xml --must:analyses \
// RUN: %builddir/tests/DatatypeTrack/analysis_spec.xml \
// RUN: %must-bin-dir/Example_4_2 2>&1 \
// RUN: | %filecheck %s

/**
 * @file example_4_2.c
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
    MPI_Datatype        types[2] = {MPI_DOUBLE, MPI_CHAR};

    MPI_Type_struct (2, blocklens, displs, types, &oldtype);
    // CHECK-DAG: Typemap = {(MPI_DOUBLE, {{[0-9]*}}), (MPI_CHAR, {{[0-9]*}})}
    test_type (oldtype);


    MPI_Type_contiguous (3, oldtype, &newtype);
    // CHECK-DAG: Typemap = {(MPI_DOUBLE, {{[0-9]*}}), (MPI_CHAR, {{[0-9]*}}), (MPI_DOUBLE, {{[0-9]*}}), (MPI_CHAR, {{[0-9]*}}), (MPI_DOUBLE, {{[0-9]*}}), (MPI_CHAR, {{[0-9]*}})}
    test_type (newtype);

    MPI_Finalize ();

    return 0;
}
