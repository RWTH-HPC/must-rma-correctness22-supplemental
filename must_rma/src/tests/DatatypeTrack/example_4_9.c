/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 1 --must:layout \
// RUN: %builddir/tests/DatatypeTrack/Example_4_9Layout.xml --must:analyses \
// RUN: %builddir/tests/DatatypeTrack/analysis_spec.xml \
// RUN: %must-bin-dir/Example_4_9 2>&1 \
// RUN: | %filecheck %s

/**
 * @file example_4_9.c
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
    int                 blocklens[3] = {1,1,1};
    MPI_Aint            displs[3] = {-3,0,6};

#ifdef HAVE_MPI_TYPE_CREATE_RESIZED
    MPI_Type_create_resized (MPI_INT, displs[0], displs[2], &type1);
#else
    MPI_Datatype        types[3] = {MPI_LB, MPI_INT, MPI_UB};
    MPI_Type_struct (3, blocklens, displs, types, &type1);
#endif
    // CHECK-DAG: Typemap = {(MPI_LB, -{{[0-9]+}}), (MPI_INT, {{[0-9]+}}), (MPI_UB, {{[0-9]+}})}
    test_type (type1);

    MPI_Type_contiguous (2, type1, &newtype);
    // CHECK-DAG: Typemap = {(MPI_LB, -{{[0-9]+}}), (MPI_INT, {{[0-9]+}}), (MPI_INT, {{[0-9]+}}), (MPI_UB, {{[0-9]+}})}
    test_type (newtype);

    MPI_Finalize ();

    return 0;
}
