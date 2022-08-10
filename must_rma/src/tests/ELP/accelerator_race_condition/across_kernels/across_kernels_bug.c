/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#pragma omp declare target
void sumup(int *sum) {
	#pragma omp parallel for num_threads(20)
	int i;
	for( i = 0; i < 100; i++ )
		*sum += i;
}

int main(int argc, char **argv) {
	int sum = 0;

	#pragma omp target data map(tofrom:sum)
	{
		#pragma omp parallel num_threads(2)
		{
			#pragma omp single

			#pragma omp target map(tofrom:sum)
			{
				sum = 1;
			}
		}
	}

	printf("Sum is: %i\n", sum);

	return 0;
}
