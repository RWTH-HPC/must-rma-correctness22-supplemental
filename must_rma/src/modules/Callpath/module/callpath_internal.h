/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PNMPI_CALLPATH_INTERNAL_H
#define PNMPI_CALLPATH_INTERNAL_H

#include "GtiTLS.h"

///\file callpath_internal.h
/// This is an internal header used for variables that are shared
/// between the callpath module and the wrappers.

/// Callpath variable is defined in the main module cpp file.
extern thread_local Callpath PNMPIMOD_Callpath_callpath;

#endif // PNMPI_CALLPATH_INTERNAL_H
