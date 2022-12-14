# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for callpath lib+module directory.
#
# @author Todd Gamblin <tgamblin@llnl.gov>
# @author Tobias Hilbrich <tobias.hilbrich@zih.tu-dresden.de>
#
# @date 18.10.2011

IF (NOT USE_CALLPATH)
    RETURN()
ENDIF ()


# Version information comes from here.  Increment these when new stuff happens.
set(CALLPATH_MAJOR_VERSION 1)
set(CALLPATH_MINOR_VERSION 0)
set(CALLPATH_PATCH_VERSION 0)
set(CALLPATH_VERSION "${CALLPATH_MAJOR_VERSION}.${CALLPATH_MINOR_VERSION}.${CALLPATH_PATCH_VERSION}")

find_package(Stackwalker)

FOREACH(path ${CALLPATH_STACKWALKER_INCLUDE_PATH})
	include_directories(${path})
ENDFOREACH(path)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/util)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/module)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

SET (CALLPATH_HAVE_MPI TRUE)
SET (CALLPATH_HAVE_SYMTAB TRUE)

# Configure a header file with all the variables we found.
configure_file(
  callpath-config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/callpath-config.h)
MUST_INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/src/callpath-config.h DESTINATION include)

# Configure subdirectories here, after everything else, so that they
# inherit variables set in the global scope.
add_subdirectory(util) 
add_subdirectory(src) 
add_subdirectory(module)
