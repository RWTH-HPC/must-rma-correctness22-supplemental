/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <omp.h>

int main(int argc, char **argv) {
	// Threads of a team reach two different barriers
	#pragma omp parallel
	{
  		if (omp_get_thread_num() % 2 == 0) {
			#pragma omp barrier
		} else {
			#pragma omp barrier
		}

	} // end omp parallel

	return 0;
}


