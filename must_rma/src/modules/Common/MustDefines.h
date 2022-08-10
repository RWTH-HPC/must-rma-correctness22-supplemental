/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MustDefines.h
 *       @see MustDefines.
 *
 *  @date 17.08.2011
 *  @author Mathias Korepkat
 */

#include "mpi.h"
#include <stdlib.h>
#include <stdint.h>

#ifndef MUSTDEFINES_H
#define MUSTDEFINES_H

#define MUST_BOTTOM ((int64_t)-1L)
#define MUST_IN_PLACE ((int64_t)-2L)

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

/**
 * generate fortran bindings
 */
#define GENERATE_F77_BINDINGS(lower_case, \
                              upper_case, \
                              wrapper_function, \
                              signature, \
                              params) \
  EXTERN void lower_case signature; \
  EXTERN void lower_case signature { wrapper_function params; } \
  EXTERN void lower_case##_ signature; \
  EXTERN void lower_case##_ signature { wrapper_function params; } \
  EXTERN void lower_case##__ signature; \
  EXTERN void lower_case##__ signature { wrapper_function params; } \
  EXTERN void upper_case signature; \
  EXTERN void upper_case signature { wrapper_function params; }

#ifndef MUST_MAX_NUM_STACKLEVELS
#define MUST_MAX_NUM_STACKLEVELS 10
#endif

#ifndef MUST_MAX_TOTAL_INFO_SIZE
#define MUST_MAX_TOTAL_INFO_SIZE 4096
#endif

inline
char* getBaseOutputDir()
{
  static char defaultOutputFile[] = ".";
  static char* outputFile = NULL;
  if (!outputFile) {
    outputFile = getenv("MUST_OUTPUT_PATH");
    if (!outputFile)
      outputFile = defaultOutputFile;
  }
  return outputFile;
}

#define MUST_OUTPUT_DIR (std::string(std::string(getBaseOutputDir()) + "/" + "MUST_Output-files/").c_str())
#define MUST_OUTPUT_REDIR "../"
#define MUST_OUTPUT_DIR_CHECK             struct stat sd;\
            if(stat(MUST_OUTPUT_DIR, &sd)!=0)\
            {\
                mkdir(MUST_OUTPUT_DIR, 0755);\
            }\

#define MUST_DIR_CHECK(path)             struct stat sd;\
            if(stat(path, &sd)!=0)\
            {\
                mkdir(path, 0755);\
            }\

const char *const MUST_STDOUT_PREFIX_REPORT = "[MUST-REPORT] ";
const char *const MUST_STDOUT_PREFIX_RUNTIME = "[MUST-RUNTIME] ";

#endif /*MUSTDEFINES_H*/
