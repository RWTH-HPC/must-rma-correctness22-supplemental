/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/OpIsNotKnownError \
// RUN: 2>&1 \
// RUN: | %filecheck-may-segfault %s

// CHECK: [MUST-REPORT]{{.*Error:.*Argument.*[(]op[)].*is a unknown operation where a valid operation was expected}}

/**
 * @file OpIsPredefinedNoError.cpp
 * This is a a test for the analysis analysis group BasicChecks.
 *
 * Description:
 * Tries to free an unknown operation in line 58, what will cause an error.
 *
 * Positiv check: OpIsPredefinedNoError.cpp
 *
 *  @date 26.05.2011
 *  @author Mathias Korepkat, Tobias Hilbrich
 */

#include <iostream>
#include <mpi.h>


//user defined operation that returns the sum
void myOp(int *invec, int *inoutvec, int *len, MPI_Datatype *dtype)
{
    for ( int i=0; i<*len; i++ )
        inoutvec[i] += invec[i];
}

int main (int argc, char** argv)
{
	int size, rank;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	MPI_Status status;

	//Enough tasks ?
	if (size < 2)
	{
		std::cerr << "This test needs at least 2 processes!"<< std::endl;
		MPI_Finalize();
		return 1;
	}

	//Say hello
	std::cout << "Hello, I am rank " << rank << " of " << size << " processes." << std::endl;


	//Create an Operation
	MPI_Op op,op_unknown;
	MPI_Op_create((MPI_User_function *)myOp,0,&op);

	op_unknown = op;

	//Free an Operation
	if(op != MPI_OP_NULL)
		MPI_Op_free(&op);

	if(op_unknown != MPI_OP_NULL)
		MPI_Op_free(&op_unknown);

	//Say bye bye
	std::cout << "Signing off, rank " << rank << "." << std::endl;

	MPI_Finalize ();

	return 0;
}
