/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

int main( int argc, char **argv )
{
	int sum = 23;

	// data is uninitialized on accelerator + not copied back afterwards
	#pragma omp target map(to:sum)
	{
		int i;
		for(i = 0; i < 100; i++)
			sum += i;
	}

	printf("Sum is: %i\n", sum);

	return 0;
}
