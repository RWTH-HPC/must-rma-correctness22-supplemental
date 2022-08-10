# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for request tracking test.
#
# @author Tobias Hilbrich
# @date 04.02.2011
#

##################
#Recurse into sub directories
ADD_SUBDIRECTORY (modules)

###################
#Add test case
addTestExecutable ("testRequestTrack" "testRequestTrack.c")
CONFIGURE_FILE(analysis_spec.xml ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml @ONLY)

configureTest (
    ${CMAKE_CURRENT_BINARY_DIR}/layout.xml
    "RequestCondition:MUST_RequestCondition;FinalizeNotify:MUST_Base"
    "PrintRequest:TestRequestTrack"
    "MsgLoggerHtml:MUST_Base;MsgLoggerStdOut:MUST_Base"
    2
    1
    )

IF(TEST_PREBUILDS)
    MUST_PREBUILD_CONFIGURATION(
	prebuild-testRequestTrack
	"--must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml \
         --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml \
         ${MPIEXEC_NUMPROC_FLAG} 2"
	)
ENDIF()

ADD_TEST(NAME testRequestTrack COMMAND
        bash ${MUSTRUN} 
            --must:timeout ${MUSTRUN_TIMEOUT}
            --must:mpiexec ${MPIEXEC} 
            --must:np ${MPIEXEC_NUMPROC_FLAG} 
            --must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml 
            --must:temp tempRequestTrack
            --must:clean
            --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
            ${MPIEXEC_NUMPROC_FLAG} 2  $<TARGET_FILE:testRequestTrack> 
            )
    
SET_TESTS_PROPERTIES (testRequestTrack PROPERTIES PASS_REGULAR_EXPRESSION
    "Persistent point-to-point request.*created.*activated.*canceled.*MPI_REQUEST_NULL")
