/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <omp.h>

int main(int argc, char **argv) {
	struct omp_nest_lock_t lock;

	omp_init_nest_lock(&lock);

	// Most simple deadlock when using omp locks
	#pragma omp parallel
	{
		#pragma omp sections
		{
			#pragma omp section
			{
				omp_set_nest_lock(&lock);
			}

			#pragma omp section
			{
				omp_set_nest_lock(&lock);
			}
		}
	}

	omp_destroy_nest_lock(&lock);

	return 0;
}
