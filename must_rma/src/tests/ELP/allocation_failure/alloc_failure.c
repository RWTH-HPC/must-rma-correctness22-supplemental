/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>

int main(int argc, char **argv) {
	size_t i;
	size_t iSize = (size_t)atoi(argv[1]) * 1024ULL * 1024ULL * 1024ULL;
	char *data;

	// Allocation on accelerator side may fail (too less memory)
	#pragma omp target map(to:iSize) map(alloc:data[0:iSize])
	{
		for (i = 0; i < iSize; i++)
			data[i] = 23;
	}

	return 0;
}
