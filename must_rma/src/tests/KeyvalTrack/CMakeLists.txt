# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for keyvalue tracking test.
#
# @author Tobias Hilbrich
# @date 13.05.2011
#

##################
#Recurse into sub directories
ADD_SUBDIRECTORY (modules)

###################
#Add test case
addTestExecutable ("testKeyvalTrack" "testKeyvalTrack.c")
CONFIGURE_FILE(analysis_spec.xml ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml @ONLY)

configureTest (
    ${CMAKE_CURRENT_BINARY_DIR}/layout.xml
    "FinalizeNotify:MUST_Base"
    "PrintKeyval:TestKeyvalTrack"
    "MsgLoggerHtml:MUST_Base;MsgLoggerStdOut:MUST_Base"
    2
    1
    )

IF(TEST_PREBUILDS)
    MUST_PREBUILD_CONFIGURATION(
	prebuild-testKeyvalTrack
	"--must:layout ${CMAKE_CURRENT_BINARY_DIR}/${test}layout.xml \
         --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml \
         ${MPIEXEC_NUMPROC_FLAG} 2"
	)
ENDIF()

ADD_TEST(NAME testKeyvalTrack COMMAND
        bash ${MUSTRUN} 
            --must:timeout ${MUSTRUN_TIMEOUT}
            --must:mpiexec ${MPIEXEC} 
            --must:np ${MPIEXEC_NUMPROC_FLAG} 
            --must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml 
            --must:temp tempKeyvalTrack
            --must:clean
            --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
            ${MPIEXEC_NUMPROC_FLAG} 2  $<TARGET_FILE:testKeyvalTrack> 
            )
    
SET_TESTS_PROPERTIES (testKeyvalTrack PROPERTIES PASS_REGULAR_EXPRESSION
    "MPI_TAG_UB.*MPI_WTIME_IS_GLOBAL.*Key created at.*MPI_KEYVAL_INVALID")
 
