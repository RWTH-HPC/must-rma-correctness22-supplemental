/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

int main( int argc, char **argv ) {
	int sum = 0;

	// Sumup is executed by 20 threads concurrently
	// and has a data race on 'sum'
	#pragma omp target map(tofrom:sum)
	{
		#pragma omp parallel for num_threads(20)
		int i;
		for(i = 0; i < 100; ++i)
			sum += i;
	}

	printf("Sum is: %i\n", sum);

	return 0;
}
