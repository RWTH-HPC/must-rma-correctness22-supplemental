/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <omp.h>

int main(int argc, char **argv) {
	struct omp_lock_t lock;

	// Usage of uninitialized locks
	#pragma omp parallel num_threads(2)
	{
		#pragma omp sections
		{
			#pragma omp section
			{
				omp_set_lock(&lock);
				omp_unset_lock(&lock);
			}
			#pragma omp section
			{
				omp_set_lock(&lock);
				omp_unset_lock(&lock);
			}
		}
	}

	return 0;
}
