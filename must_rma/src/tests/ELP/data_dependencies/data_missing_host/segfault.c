/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>

#pragma omp declare target
int some_func (int i) {
	return i*42;
}

int main (int argc, char **argv) {
	int *array = NULL;

	// error: copy of NULL pointer, segfault
	#pragma omp target data map(to:array[0:1024])
	{
		#pragma omp target map(to:array[0:1024])

		#pragma omp parallel for
		int i;
		for(i = 0; i < 1024; i++)
			array[i] = some_func(i);

	} // end omp target data

	return 0;
}
