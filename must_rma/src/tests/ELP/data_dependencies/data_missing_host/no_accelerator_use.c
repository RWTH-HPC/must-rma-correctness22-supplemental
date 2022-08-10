/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "not enough aruments!\nusage: %s (number) (number)\n");
		exit(EXIT_FAILURE);
	}

	int N = atoi(argv[1]);
	int M = atoi(argv[2]);

	int *array = (int*)malloc(N * sizeof(int));

	#pragma omp target data map(to:array[0:N-2])
	{
		#pragma omp target map(to:array[0:N])

		#pragma omp parallel for
		int i;
		for(i = 0; i < M; i++)
			array[i] = i*42;
	} // end omp target data

	return 0;
}
