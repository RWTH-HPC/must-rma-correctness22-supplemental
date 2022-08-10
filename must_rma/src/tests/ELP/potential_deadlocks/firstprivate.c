/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

int main(int argc, char **argv) {
	int i, j = 0;

	#pragma omp parallel for private(j) /*lastprivate(j)*/
	for( i = 0; i < 100; i++ )
		if( i % 42 == 0 )
			j = i;

	printf("%i\n", j);

	return 0;
}
