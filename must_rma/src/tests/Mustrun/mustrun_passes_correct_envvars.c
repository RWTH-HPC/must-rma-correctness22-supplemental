/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file mustrun_passes_correct_envvars.c
 * This is a test for the mustrun script.
 *
 * Description:
 * Checks if the test binary got the environment variables passed correctly.
 *
 *  @date 03.03.2022
 *  @author Sebastian Grabowski
 */

// RUN: env MUST__TESTVAR="$\"MUST rocks!" %must-run %mpiexec-numproc-flag 1 \
// RUN: %must-bin-dir/mustrun_passes_correct_envvars | %filecheck %s

// CHECK: Unknown environment option MUST__TESTVAR=$"MUST rocks! is ignored
// CHECK-NOT: {{^}}ERROR:
// CHECK: {{^}}OK

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
  // The mpi calls are only here to prevent possible confusion of must. It seems
  // to timeout if called with the unusual case of only one mpi process.
  int rank = -1;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  const char *const value = getenv("MUST__TESTVAR");
  // Test if the variable exists
  if (value == NULL) {
    printf("ERROR: the environment variable MUST__TESTVAR is not set");
    goto fail;
  }

  // Check if the variable has the expected value
  const char * const expected = "$\"MUST rocks!";
  if (strncmp(value, expected, strlen(value)) != 0) {
    printf("ERROR: the environment variable MUST__TESTVAR has an unexpected "
           "value:\n"
           "  expected: MUST__TESTVAR=\"%s\"\n"
           "  actual:   MUST__TESTVAR=\"%s\"\n",
           expected, value);
    goto fail;
  }
  printf("OK\n");

  MPI_Finalize();
  return 0;
fail:
  MPI_Finalize();
  return 1;
}
