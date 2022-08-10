/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>

#pragma omp declare target
int some_func(int i) {
	return i*42;
}

int main( int argc, char **argv ) {
	int N = atoi(argv[1]);
	int i;
	int *array = (int*)malloc(N * sizeof(int));

	// possible error if N < 1024
	#pragma omp target data map(to:array[0:N])
	{
		#pragma omp target map(to:array[0:N])

		#pragma omp parallel for
		for(i = 0; i < 40960; i++)
			array[i] = some_func(array[i]);
	} // end omp target data

	return 0;
}
