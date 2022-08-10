/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file datatypetest.h
 * Helper functions, headers
 *
 * @author Joachim Protze
 */

#ifndef DATATYPE_TEST_H
#define DATATYPE_TEST_H

#include "stdlib.h"
#include "stdio.h"
#include "mpi.h"
#include "mustFeaturetested.h"

// #define datatypeTest advancedTest
#define datatypeTest noopTest

void printIntMap(MPI_Datatype type);

void printTypeAttributes(MPI_Datatype type);

void advancedTest(MPI_Datatype type);
void noopTest(MPI_Datatype type);

int mpiResizedByStruct(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype* newtype);

int mpiResizedByStructLimits(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype* newtype);

int mpiResizedByHindexed(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype* newtype);

#endif /*DATATYPE_TEST_H*/
