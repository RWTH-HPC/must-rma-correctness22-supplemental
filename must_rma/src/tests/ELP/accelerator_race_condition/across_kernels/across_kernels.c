/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#pragma omp declare target
void sumup(int *sum) {
	int i;
	for(i = 0; i < 100; i++) {
		*sum += i;
	}
}

int main(int argc, char **argv) {
	int sum = 0;

	// Sumup is started in two kernels at the same time
	// over a sections construct inside a parallel region
	#pragma omp target data map(tofrom:sum)
	{
		#pragma omp parallel num_threads(2)
		{
			#pragma omp sections
			{
				#pragma omp section
				#pragma omp target map(tofrom:sum)
				{
					sumup(&sum);
				}
				#pragma omp section
				#pragma omp target map(tofrom:sum)
				{
					sumup(&sum);
				}
			}
		}
	}

	printf("Sum is: %i\n", sum);

	return 0;
}
