/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
	// Not all threads may reach the omp barrier
	#pragma omp parallel num_threads(4)
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		if(tv.tv_usec % 2 == 0) {
			#pragma omp barrier
		}
	}
	return 0;
}
