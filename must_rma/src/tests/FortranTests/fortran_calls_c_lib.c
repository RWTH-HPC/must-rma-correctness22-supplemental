/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file fortranc_C.cpp
 * This is a a test for the analysis group BasicChecks.
 *
 *  Description:
 *  A function that performs a MPI_Recv. This Routine is used by fortranc.f
 *
 *  @date 16.08.2011
 *  @author Mathias Korepkat
 */
#include <mpi.h>
#include "MustDefines.h"

void MyRecv (int* out)
{
	MPI_Status status;
	MPI_Recv (out, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, &status);
}GENERATE_F77_BINDINGS(myrecv, MYRECV, MyRecv, (int* out), (out))
