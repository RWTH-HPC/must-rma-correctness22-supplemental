/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file datatypeTest.h
 * Testoutput for datatypes.
 *
 * @author Joachim Protze
 */

#include <mpi.h>
#include <stdio.h>
#include "mustTest.h"
#include "mustFeaturetested.h"
#include "MustTypes.h"

#ifndef DATATYPETEST_H
#define DATATYPETEST_H

/**
 * Removed the inline below, this causes problems with llvm based compilers that assume C99 per default.
 * In that case another definition for the function must exist, which won't hold in this case.
 * The inline shouldn't be that important here.
 */
/*inline*/
void test_btype(MPI_Datatype datatype)
{
    MustDatatypeType fType = MUST_Type_m2i (datatype);
    MPI_Initialized ((int*)&fType);
}

/**
 * Removed the inline below, this causes problems with llvm based compilers that assume C99 per default.
 * In that case another definition for the function must exist, which won't hold in this case.
 * The inline shouldn't be that important here.
 */
/*inline*/
void test_type(MPI_Datatype datatype)
{
    MustDatatypeType fType = MUST_Type_m2i (datatype);
    int size;
    MPI_Aint lb, extent, ub;
    MPI_Initialized ((int*)&fType);
    MPI_Type_commit(&datatype);
    MPI_Type_size(datatype, &size);
    printf("\nMPI_Type_size(%i)\n",size);
#ifdef HAVE_MPI_TYPE_GET_EXTENT
    MPI_Type_get_extent(datatype, &lb, &extent);
    printf("MPI_Type_get_extent(%li, %li)\n", lb, extent);
#endif
#ifdef HAVE_MPI_TYPE_GET_TRUE_EXTENT
    MPI_Type_get_true_extent(datatype, &lb, &extent);
    printf("MPI_Type_get_true_extent(%li, %li)\n", lb, extent);
#endif
    MPI_Type_lb(datatype, &lb);
    printf("MPI_Type_lb(%li)\n", lb);
    MPI_Type_extent(datatype, &extent);
    printf("MPI_Type_extent(%li)\n", extent);
    MPI_Type_ub(datatype, &ub);
    printf("MPI_Type_ub(%li)\n\n", ub);
    fType = MUST_Type_m2i (datatype);
    MPI_Initialized ((int*)&fType);
}
#endif /*DATATYPETEST_H*/
