# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.txt
#  CMake file for ELP directory
#
# @author Alexander Haase @date 06.05.2014
#

IF (NOT ENABLE_ELP_EXTENSIONS)
    RETURN()
ENDIF ()

# Recurse into sub directories
ADD_SUBDIRECTORY(data_dependencies)
ADD_SUBDIRECTORY(accelerator_race_condition)
ADD_SUBDIRECTORY(potential_deadlocks)
ADD_SUBDIRECTORY(api_missusage)
ADD_SUBDIRECTORY(allocation_failure)
