/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

int main(int argc, char **argv) {
	int a = 42;
	int b = 17;
	int sum = 0;

	// a and b are uninitialized on accelerator
	#pragma omp target map(alloc:a,b) map(tofrom:sum)
	{
		sum = a + b;
	}

	printf("Sum is: %i\n", sum);

	return 0;
}
