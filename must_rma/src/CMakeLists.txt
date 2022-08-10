# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause
#
# @file CMakeLists.cmake
#       MUST core CMake file.
#
# @author Tobias Hilbrich
# @author Alexander Haase <alexander.haase@rwth-aachen.de>
#
# @date 23.06.2014

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

#===================================
# Policies
#===================================
FOREACH (POL CMP0048 CMP0074)
    IF (POLICY ${POL})
        CMAKE_POLICY(SET ${POL} NEW)
    ENDIF ()
ENDFOREACH ()


#===================================
# Basic initialization
#===================================

PROJECT(MUST C CXX)

SET(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmakemodules"
    "${CMAKE_SOURCE_DIR}/externals/CMake-gitinfo/cmake"
    "${CMAKE_SOURCE_DIR}/externals/CMake-gitpack/cmake"
    "${CMAKE_SOURCE_DIR}/externals/CMake-MPIhelper/cmake"
    ${CMAKE_MODULE_PATH}
)

# Get the version info from git.
FIND_PACKAGE(GitInfo REQUIRED)
GIT_VERSION_INFO(MUST REQUIRED)

# Optional Fortran support.
#
# By default MUST and its dependencies will be built with Fortran support
# enabled. This requires a Fortran compiler and Fortran-enabled versions of MPI,
# GTI and PnMPI. If one of the conditions is not matched, or one wants to build
# MUST without Fortran support, 'ENABLE_FORTRAN' needs to be disabled at
# configuration time.
OPTION(ENABLE_FORTRAN "Build MUST with Fortran support." ON)
IF (ENABLE_FORTRAN)
    ENABLE_LANGUAGE(Fortran)
ENDIF ()

OPTION(ENABLE_TSAN "Enable support for analysis with TSAN." TRUE)

# Variable to decide whether TSan tests can be built
SET (HAVE_TSAN False)
IF (ENABLE_TSAN)
  MESSAGE(STATUS "Looking for ThreadSanitizer")
  IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      IF (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8)
        SET (HAVE_TSAN True)
      ENDIF()
  ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      IF (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
        SET (HAVE_TSAN True)
      ENDIF()
  ENDIF()
  IF(HAVE_TSAN)
    MESSAGE(STATUS "Looking for ThreadSanitizer - found")
  ELSE()
    MESSAGE(STATUS "Looking for ThreadSanitizer - not found")
  ENDIF()
ENDIF (ENABLE_TSAN)

#===================================
#Warnings
#===================================
OPTION (ENABLE_WARNINGS "Selects whether compiler warnings are enabled." FALSE)
IF (ENABLE_WARNINGS)
  SET(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
  SET(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")
ENDIF (ENABLE_WARNINGS)


#Include helper modules
INCLUDE (MTHelperMacros)

#Set prefix
#CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT is a hack and can be googled, not sure whether it is going to stay
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  IF (WIN32)
    SET(CMAKE_INSTALL_PREFIX "$ENV{PROGRAMFILES}/must" CACHE PATH "Installation directory" FORCE)
  ELSE (WIN32)
    SET(CMAKE_INSTALL_PREFIX "/usr/local/must" CACHE PATH "Installation directory" FORCE)
  ENDIF (WIN32)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#Binary dir stuff
SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib CACHE PATH "Directory for built libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for built executables.")
MARK_AS_ADVANCED (LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#Backup of compilers
SET (MUST_C_COMPILER ${CMAKE_C_COMPILER} CACHE INTERNAL "")
SET (MUST_CXX_COMPILER ${CMAKE_CXX_COMPILER} CACHE INTERNAL "")
SET (MUST_Fortran_COMPILER ${CMAKE_Fortran_COMPILER} CACHE INTERNAL "")

OPTION(USE_CALLPATH "Enable stacktraces with the stackwalker from dyninst, this requires you to set multiple paths that start with \"CALLPATH_\", as there is no easy way to link to that yet." FALSE)

# TODO RMA: Disable backward by default due to incompatibility with codeptr changes
OPTION(USE_BACKWARD "Enable stacktraces with backward-cpp." FALSE)

#===================================
# Compiler options
#===================================

# Set MPI skip flags for C++ code
#
# Some MPI implementations use a legacy C++ header for mapping MPI C calls to
# C++ methods. Sometimes, code compiled with this "automatic feature" fails when
# executed due unresolvable symbols. The following flags will disable this
# "feature" entirely for all C++ code compiled in this project.
set(MPI_CXX_SKIP_FLAGS "-DMPI_NO_CPPBIND"      # SGI
                       "-DOMPI_SKIP_MPICXX"    # OpenMPI
                       "-D_MPICC_H"            # HP-MPI
                       "-DMPICH_SKIP_MPICXX"   # MPICH
                       "-DMPIBULL_SKIP_MPICXX" # BUll-MPI
    CACHE STRING "Flags to skip C++ MPI")
string(REPLACE ";" " " CMAKE_CXX_FLAGS
       "${CMAKE_CXX_FLAGS} ${MPI_CXX_SKIP_FLAGS}")


##===================================
## Check Python3 support
##===================================
#TODO: If CMAKE_MINIMUM_VERSION >= 3.12 required use FindPython3
# As long as FindPython3 is not used, keep this check before checking
# for Python2 (as e.g. in externals/wrap)
SET(Python_ADDITIONAL_VERSIONS 3)
FIND_PACKAGE(PythonInterp 3 REQUIRED)

#===================================
# System inspection
#===================================

# Search for general required packages.
#
# Some packages are required in several subdirectories. These will be searched
# here inside the project's root, so all subdirectories may use the cached
# results. Otherwise CMake may slow down, as e.g. 'FIND_PACKAGE(MPI)' gets
# called several times for the feature tests, which can't share the results if
# called inside the feature test.
FIND_PACKAGE(MPI REQUIRED)

# Search for dependencies with fallbacks in the 'externals' subdirectory.
#
# Some of MUST's dependencies (like GTI and PnMPI) provide their resources as
# IMPORTED targets. As these are not available in parent scope and the
# dependencies can't be searched multiple times because of their fallbacks from
# the submodules, these need to be searched the project's root. By including the
# CMakeLists of 'externals' subdirectory, these will become visible for the
# entire project.
#
# NOTE: These dependencies need to be searched before recusing into the
#       subdirectories, so these are visible in them.
include(externals/CMakeLists.txt)

# Check if GTI matches the configuration of MUST.
#
# If GTI doesn't support features required by the current MUST configuration,
# abort configuring MUST and tell the user what features can't be used.
IF (ENABLE_FORTRAN AND NOT GTI_ENABLE_FORTRAN)
    MESSAGE(FATAL_ERROR "The Fortran features of MUST require a version of GTI "
                        "with Fortran support enabled.")
ENDIF ()


#===================================
#Find necessary software/libraries/environmental stuff
#===================================
FIND_PACKAGE(AWK REQUIRED)
FIND_PACKAGE(LDD QUIET)
FIND_PACKAGE(OTOOL QUIET)
FIND_PACKAGE(MD5SUM QUIET)
FIND_PACKAGE(UnixCommands REQUIRED)
FIND_PACKAGE(DOT)
FIND_PACKAGE(OpenMP QUIET)
IF (NOT OPENMP_FOUND)
  MESSAGE (STATUS "Compiler support for OpenMP not found - disabling optional OpenMP features")
ENDIF (NOT OPENMP_FOUND)

IF ( NOT OTOOL_FOUND AND NOT LDD_FOUND )
      MESSAGE(FATAL_ERROR  "Could neither find a ldd nor an otool, please install either one!")
ENDIF( NOT OTOOL_FOUND AND NOT LDD_FOUND )
IF (USE_CALLPATH) ##The prefix/postifx is used in XML specification to adapt them to the availability of callpaths
	FIND_PACKAGE(Stackwalker)
  SET (ENABLE_STACKTRACE TRUE)
ENDIF()
IF (USE_BACKWARD)
  LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/externals/backward-cpp)
  FIND_PACKAGE(Backward)
  SET (ENABLE_STACKTRACE TRUE)
ENDIF()



#===================================
# Set DTD-Path for specifications
#===================================
GET_FILENAME_COMPONENT(MUST_SPECIFICATION_DTD_PATH
                       "${GTI_SPECIFICATION}" DIRECTORY)


#===================================
#Include sub-directories
#===================================
IF (ENABLE_STACKTRACE) ##The prefix/postifx is used in XML specification to adapt them to the availability of callpaths
    SET (STACKTRACE_XML_PREFIX "")
    SET (STACKTRACE_XML_SUFFIX "")
ELSE (ENABLE_STACKTRACE)
    SET (STACKTRACE_XML_PREFIX "<!--")
    SET (STACKTRACE_XML_SUFFIX "-->")
ENDIF (ENABLE_STACKTRACE)
INCLUDE (MustFeaturetests)
ADD_SUBDIRECTORY(specifications)
ADD_SUBDIRECTORY(doxygen)
ADD_SUBDIRECTORY(prebuild)
ADD_SUBDIRECTORY(utility)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(modules)

##===================================
## Generate the include header for user annotations with the help of GTI's helper script.
##===================================
SET(annotationXML ${CMAKE_BINARY_DIR}/specifications/must_annotation_api_spec.xml)
SET(annotationHeader ${CMAKE_BINARY_DIR}/include/GTI_Annotations.h)
SET(annotationScript ${CMAKE_SOURCE_DIR}/externals/GTI/utility/generate_annotation_header.py)
ADD_CUSTOM_TARGET(annotationDummy ALL COMMAND "" DEPENDS ${annotationXML} ${annotationScript})
ADD_CUSTOM_COMMAND(TARGET annotationDummy POST_BUILD
                  COMMAND ${PYTHON_EXECUTABLE} ${annotationScript} -s ${annotationXML} -d ${CMAKE_BINARY_DIR}/include
                  COMMENT "Generating user annotation header ${annotationHeader}"
                  VERBATIM)
INSTALL(FILES ${annotationHeader} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

#===================================
#Tests
#===================================
OPTION (ENABLE_TESTS "Selects whether tests are built." FALSE)
IF (ENABLE_TESTS)
    OPTION (DISABLE_UMPIRE_TESTS "Selects whether umpire tests are disabled." FALSE)
    OPTION (DISABLE_MARMOT_TESTS "Selects whether marmot tests are disabled." FALSE)
    INCLUDE (CTest)
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(tests)
ENDIF (ENABLE_TESTS)


# CPack configuration for packaging sources.
#
# If MUST is NOT included into other projects (via add_subdirectory()),
# configure CPack to pack the sources. The package may be generated by building
# the 'package_deploy' target.
if (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CPACK_PACKAGE_VERSION          "${MUST_VERSION}")
  set(CPACK_PACKAGE_VERSION_MAJOR    ${MUST_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR    ${MUST_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH    ${MUST_VERSION_PATCH})
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "MUST-${MUST_VERSION}")

  include(GitPack)
  include(CPackDeploy)
endif ()
