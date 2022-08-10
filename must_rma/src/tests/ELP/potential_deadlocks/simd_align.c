/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	double *a = malloc(1024 * sizeof(double));
	double *b = malloc(1024 * sizeof(double));
	double *c = malloc(1024 * sizeof(double));

	int i;
	for(i = 0; i < 1024; i++) {
		a[i] = i*2;
		b[i] = i*42;
		c[i]= 0;
	}

	// usage of aligned with unaligned data
	#pragma omp simd aligned(a,b,c:8) safelen(16)
	for(i = 0; i < 1024; i++)
		c[i] = a[i] * b[i];

	printf("%f\n", c[1]);

	return 0;
}
