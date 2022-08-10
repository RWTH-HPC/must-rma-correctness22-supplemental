# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for tests directory.
#
# @author Tobias Hilbrich
# @date 03.01.2011
#
# The tests have to be used as follows:
#    ccmake       #configure with CMake, enable the test option there, generate
#    make         #build
#    make install #install and patch the libraries
#    make test    #run the tests
#
# Currently likely not very portable, should work well with OpenMPI atm.
#

##################
#Recurse into sub directories

#Add Include directories for modules
INCLUDE_DIRECTORIES(
    "${CMAKE_BINARY_DIR}/modules/Common" #For results of feature tests
    "${CMAKE_SOURCE_DIR}/modules/MustBase"  #For MustParallelId and the like
    "${CMAKE_SOURCE_DIR}/modules/ResourceTracking/Comm" 
    "${CMAKE_SOURCE_DIR}/modules/ResourceTracking/Datatype"
    "${CMAKE_SOURCE_DIR}/modules/ResourceTracking/Err"
    "${CMAKE_SOURCE_DIR}/modules/ResourceTracking/Group"
    "${CMAKE_SOURCE_DIR}/modules/ResourceTracking/Keyval"
    "${CMAKE_SOURCE_DIR}/modules/ResourceTracking/Op"
    "${CMAKE_SOURCE_DIR}/modules/ResourceTracking/Request"
    "${CMAKE_SOURCE_DIR}/modules/ResourceTracking/Utility"
    "${CMAKE_SOURCE_DIR}/modules/Common"
    "${GTI_INCLUDE_PATH}" # For I_Module and the like
    "${GTI_SOURCE_PATH}"  # For gti::ModuleBase and the like
    ${PnMPI_INCLUDE_DIR}  # Used by gti::ModuleBase
    ${MPI_C_INCLUDE_PATH}      # For mpi.h
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include # For GTI_Annotations.h
    )

#Build base sources variable for modules
SET (BASE_SOURCES)
SET (MUSTRUN "${CMAKE_BINARY_DIR}/utility/mustrun")


# Recurse into all subdirectories.
#
# NOTE: Subdirectories will be searched dynamically to support excluding
#       specific test cases by simply removing their folders (i.e. to not ship
#       experimental tests on deploy).
FILE(GLOB SUBDIRECTORIES "*")
IF (NOT ENABLE_TSAN)
  list(REMOVE_ITEM SUBDIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/MpiTSan")
ENDIF (NOT ENABLE_TSAN)
# Exclude the unit folder as it will be added conditionally below
list(REMOVE_ITEM SUBDIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/unit")
FOREACH (DIR ${SUBDIRECTORIES})
    IF (EXISTS "${DIR}/CMakeLists.txt")
        ADD_SUBDIRECTORY(${DIR})
    ENDIF ()
ENDFOREACH ()

# Set up lit based tests

#[[
Determine the version of llvm-lit that the variable LLVM_LIT_PATH
points to. It sets the variables LLVM_LIT_VERSION,
LLVM_LIT_VERSION_MAJOR, LLVM_LIT_VERSION_MINOR and
LLVM_LIT_VERSION_PATCH.
#]]
function(find_lit_version)
    execute_process(
	COMMAND "${LLVM_LIT_PATH}" --version
	OUTPUT_VARIABLE stdout
	)
    string(REGEX MATCH [[^lit ([0-9]+)\.([0-9]+)\.([0-9])+]] MATCH "${stdout}")

    if(NOT MATCH)
	message(SEND_ERROR "Could not determine the lit version.")
    endif()

    set(LLVM_LIT_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(LLVM_LIT_VERSION_MINOR ${CMAKE_MATCH_2})
    set(LLVM_LIT_VERSION_PATCH ${CMAKE_MATCH_3})
    set(LLVM_LIT_VERSION "${LLVM_LIT_VERSION_MAJOR}.${LLVM_LIT_VERSION_MINOR}.${LLVM_LIT_VERSION_PATCH}")

    set(LLVM_LIT_VERSION_MAJOR ${LLVM_LIT_VERSION_MAJOR} PARENT_SCOPE)
    set(LLVM_LIT_VERSION_MINOR ${LLVM_LIT_VERSION_MINOR} PARENT_SCOPE)
    set(LLVM_LIT_VERSION_PATCH ${LLVM_LIT_VERSION_PATCH} PARENT_SCOPE)
    set(LLVM_LIT_VERSION ${LLVM_LIT_VERSION} PARENT_SCOPE)
endfunction()

find_package(LLVM CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

find_program(LLVM_LIT_PATH
  NAMES llvm-lit lit lit.py llvm-lit.py
  HINTS ${LLVM_TOOLS_BINARY_DIR}
  DOC "llvm-lit executable."
)
if(NOT LLVM_LIT_PATH)
  message(SEND_ERROR "Could not find llvm's lit executable needed for testing. \
Please make shure that either the LLVM package is in your CMake module search \
path or that the executable is in your PATH environment variable. \
Alternatively you might want to set the CMake variable LLVM_TOOLS_BINARY_DIR \
to the path containing the lit executable.")
endif()
find_lit_version()
if(LLVM_LIT_VERSION VERSION_LESS 12)
    message(SEND_ERROR "llvm-lit version >=12.0.0 is required when ENABLE_TESTS is set.")
endif()

find_program(LLVM_FILECHECK_PATH
  NAMES Filecheck FileCheck filecheck
  HINTS ${LLVM_TOOLS_BINARY_DIR} 
  DOC "Filecheck executable."
)
if(NOT LLVM_FILECHECK_PATH)
  message(SEND_ERROR "Could not find llvm's FileCheck executable needed for \
testing. Please make shure that either the LLVM package is in your CMake \
module search path or that the executable is in your PATH environment \
variable. Alternatively you might want to set the CMake variable \
LLVM_TOOLS_BINARY_DIR to the path containing the FileCheck executable.")
endif()

macro(pythonize_bool var)
  if (${var})
    set(${var} True)
  else()
    set(${var} False)
  endif()
endmacro()

set(MUST_TEST_MPIC_COMPILER "mpicc" )
set(MUST_TEST_COMPILER_FEATURES "")
set(MUST_FILECHECK_EXECUTABLE "${LLVM_FILECHECK_PATH}")
set(MUST_TEST_FLAGS "-rdynamic")
set(MUST_MUSTRUN_EXECUTION_FLAGS "--must:mpiexec orterun --must:output stdout" )
set(MUST_LIT_EXECUTABLE "${LLVM_LIT_PATH}")
set(MUST_MUSTRUN "${CMAKE_INSTALL_PREFIX}/bin/mustrun")

set(TESTS_WORKERS 1 CACHE STRING "Sets the amount of workers when running tests. (Note that this only sets the amount of lit workers of which each might spawn more processes.)")

####################################################
## Macro must_add_check_suite
##
## Creates a check target that executes a lit
## subsuite. The resulting target has the name
## check-${suite}.
##
## The suite named "all" has a special meaning and
## executes all tests. Its resulting target is
## simply called "check".
####################################################
function(must_add_check_suite suite)
  message(DEPRECATION "The function must_add_check_suite is deprecated. Better use must_add_check_target instead.")
  # Simulate behaviour of the old function
  if("${suite}" STREQUAL "all")
    must_add_check_target(NAME "check" SUITES "." COMMENT "Run all MUST tests")
  else()
    must_add_check_target(NAME check-${suite} SUITES ${suite} COMMENT "Run MUST::${suite} test suite")
  endif()
endfunction()

########################################################################
#
# Creates a check target that executes lit test suites.
#
# Signature:
# ```
# must_add_check_target(
#   SUITES lit_test_path1 [lit_test_path2 ...]
#   [NAME target_name]
#   [COMMENT comment]
#   [FAST_TESTS]
#   )
# ```
#
# The options are:
# - `SUITES`: Required. One or more paths of lit test suites.
# - `NAME`: The name of the target. Defaults to `check-<lit_test_path1>`.
# - `COMMENT`: The comment displayed by the target.
# - `FAST_TESTS`: Selects only fast tests by passing the environment
#   variable `MUST_FAST_TESTS_ONLY` to llvm-lit.
#
########################################################################
function(must_add_check_target)
  # Let CMakes's parser do the heavy lifting
  set(options FAST_TESTS)
  set(oneValueArgs NAME COMMENT)
  set(multiValueArgs SUITES)
  cmake_parse_arguments(PARSE_ARGV 0 ADD_CHECK_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}")

  # Again some input validation. Devs cannot be trusted.
  list(LENGTH ADD_CHECK_TARGET_SUITES suiteCount)
  if(suiteCount EQUAL 0)
    message(FATAL_ERROR "You have to pass at least one lit test suite path.")
  endif()

  # Process the optional target name argument
  if(ADD_CHECK_TARGET_NAME)
    set(target-name ${ADD_CHECK_TARGET_NAME})
  else()
    list(GET ADD_CHECK_TARGET_SUITES 0 firstSuite)
    set(target-name check-${firstSuite})
  endif()

  if(NOT ADD_CHECK_TARGET_COMMENT)
    list(GET ADD_CHECK_TARGET_SUITES 0 firstSuite)
    set(ADD_CHECK_TARGET_COMMENT "Running test suites: MUST::${firstSuite}")
    # Get the lists tail
    set(tmpList ${ADD_CHECK_TARGET_SUITES})
    list(REMOVE_AT tmpList 0)
    foreach(suite IN LISTS tmpList)
      set(ADD_CHECK_TARGET_COMMENT "${ADD_CHECK_TARGET_COMMENT}, MUST::${suite}")
    endforeach()
  endif()

  # Process flags
  if(ADD_CHECK_TARGET_FAST_TESTS)
    set(envVars ${envVars} MUST_FAST_TESTS_ONLY=1)
  endif()

  # Prepare escaped paths (who doesn't love spaces in paths?)
  foreach(suite_path IN LISTS ADD_CHECK_TARGET_SUITES)
    set(suitePaths ${suitePaths} "${suite_path}")
  endforeach()
  add_custom_target("${target-name}"
    COMMAND ${CMAKE_COMMAND} -E env ${envVars}
    "${MUST_LIT_EXECUTABLE}"
    --output "${target-name}.out.json"
    --xunit-xml-output "${target-name}.xunit.xml"
    --verbose
    --workers ${TESTS_WORKERS}
    --time-tests
    ${suitePaths}
    VERBATIM
    BYPRODUCTS ${target-name}.out.json ${target-name}.xunit.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "${ADD_CHECK_TARGET_COMMENT}"
    USES_TERMINAL)
endfunction()

set(suites
    all
    Annotations
    basic
    BasicChecks
    BufferChecks
    CommChecks
    CommGroupTrack
    DatatypeChecks
    DatatypeImplementation
    DatatypeTrack
    DeadlockDetection
    ErrTrack
    ErrorFilter
    FortranTests
    GroupChecks
    Hybrid
    IntegrityChecks
    KeyvalTrack
    LeakChecks
    OneSidedChecks
    Mustrun
    MpiTSan
    OnlyOnRootCondition
    OpTrack
    OverlapChecks
    RequestChecks
    RequestTrack
    TypeArt
    TypeMatch
    VectorClock
    unit
)
foreach(suite ${suites})
    must_add_check_target(SUITES ${suite})
endforeach()
must_add_check_target(NAME check SUITES . COMMENT "Run all MUST tests")
must_add_check_target(NAME check-fast SUITES . FAST_TESTS COMMENT "Run all fast MUST tests")

# Small set of tests to check basic build & run functionality.
must_add_check_target(
        NAME check-minimal
        COMMENT "Run a small selection of tests"
        SUITES
        basic/initialized.c  # simple test without layout
        basic/basic.c  # simple test with layout
        MpiTSan/send.c  # non-distributed
        MpiTSan/bcast.c  # distributed
        DeadlockDetection/DeadlockDetection/sendDl.c  # local & distributed deadlock
        Hybrid/CommSizeOnWorkerThread.cpp  # basic hybrid check
        FortranTests/simple_fortran.f  # fortran based test
        OpTrack/testOpTrack.c  # Track test
        Annotations/AnnotationWarningIfZero.cpp  # GTI Annotations
        TypeArt/DistributedStructError.c  # TypeArt
        ErrorFilter/single_error.c  # Message filter
)

if(NOT DISABLE_MARMOT_TESTS)
    must_add_check_target(SUITES MarmotTests)
endif()
if(NOT DISABLE_UMPIRE_TESTS)
    must_add_check_target(SUITES umpireTests)
endif()

# Unit tests
find_package(GTest 1.8.1)

if(GTest_FOUND)
    find_lit_version()
    if(LLVM_LIT_VERSION VERSION_GREATER_EQUAL 14)
	add_subdirectory(unit)
    else()
	# See: https://github.com/llvm/llvm-project/commit/14a793ab0589de48511455038f63f28ff2506d48
	message(STATUS "llvm-lit below version 14.0.0 is not compatible with current versions of GoogleTest. Disabling unit tests.")
    endif()
else()
    message(STATUS "Could not find the googletest library. Disabling unit tests.
Hint: To install googletest, run on
 - Fedora: dnf install 'cmake(GTest)'
 - Ubuntu: apt install libgtest-dev")
endif()

OPTION (TESTS_DISABLE_MUST_CLEAN "Selects whether the mustrun option '--must:clean' is disabled during test runs." False)
if(ENABLE_PREBUILD)
    OPTION (TEST_PREBUILDS "Selects whether the prebuilds for tests should be generated. This accelerates test execution." False)
endif()

OPTION (MUST_FAST_TESTS_ONLY "Run only fast executing tests." False)

set(MUST_MPI_HAS_ASSERTIONS False CACHE BOOL "Set this to true when the MPI library itself has builtin assertions.")
mark_as_advanced(FORCE MUST_MPI_HAS_ASSERTIONS)

set(variables_used_in_lit
    GTI_IS_THREAD_SAFE
    OpenMP_FOUND
    HAVE_MPI_TYPE_CREATE_RESIZED
    HAVE_MPI_TYPE_CREATE_INDEXED_BLOCK
    HAVE_MPI_ALLTOALLW
    HAVE_MPI_EXSCAN
    HAVE_MPI_REDUCE_SCATTER_BLOCK
    HAVE_MPI_REDUCE_SCATTER
    HAVE_MPI_WCHAR
    HAVE_MPI_SIGNED_CHAR
    HAVE_MPI_IALLGATHER
    HAVE_MPI_IALLGATHERV
    HAVE_MPI_IALLREDUCE
    HAVE_MPI_IALLTOALL
    HAVE_MPI_IALLTOALLV
    HAVE_MPI_IALLTOALLW
    HAVE_MPI_IBARRIER
    HAVE_MPI_IBCAST
    HAVE_MPI_IGATHER
    HAVE_MPI_IGATHERV
    HAVE_MPI_IREDUCE
    HAVE_MPI_IREDUCE_SCATTER
    HAVE_MPI_IREDUCE_SCATTER_BLOCK
    HAVE_MPI_ISCAN
    HAVE_MPI_IEXSCAN
    HAVE_MPI_ISCATTER
    HAVE_MPI_ISCATTERV
    TESTS_DISABLE_MUST_CLEAN
    DISABLE_MARMOT_TESTS
    DISABLE_UMPIRE_TESTS
    GTI_ENABLE_FORTRAN
    ENABLE_Fortran_MIXED_TESTS
    ENABLE_Fortran_TESTS
    MUST_FAST_TESTS_ONLY
    MUST_MPI_HAS_ASSERTIONS
    ENABLE_TYPEART
    HAVE_TSAN
    ENABLE_STACKTRACE
)
foreach(var ${variables_used_in_lit})
    pythonize_bool(${var})
endforeach()

configure_file(lit.site.cfg.in lit.site.cfg @ONLY)
