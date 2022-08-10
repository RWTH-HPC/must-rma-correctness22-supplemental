# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for op tracking test.
#
# @author Tobias Hilbrich
# @date 10.05.2011
#

##################
#Recurse into sub directories
ADD_SUBDIRECTORY (modules)

###################
#Add test case
addTestExecutable ("testOpTrack" "testOpTrack.c")
CONFIGURE_FILE(analysis_spec.xml ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml @ONLY)

configureTest (
    ${CMAKE_CURRENT_BINARY_DIR}/layout.xml
    "FinalizeNotify:MUST_Base"
    "PrintOp:TestOpTrack"
    "MsgLoggerHtml:MUST_Base;MsgLoggerStdOut:MUST_Base"
    2
    1
    )

IF(TEST_PREBUILDS)
    MUST_PREBUILD_CONFIGURATION(
	prebuild-testOpTrack
	"--must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml \
         --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml \
         ${MPIEXEC_NUMPROC_FLAG} 2"
	)
ENDIF()

ADD_TEST(NAME testOpTrack COMMAND
        bash ${MUSTRUN} 
            --must:timeout ${MUSTRUN_TIMEOUT}
            --must:mpiexec ${MPIEXEC} 
            --must:np ${MPIEXEC_NUMPROC_FLAG} 
            --must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml 
            --must:temp tempOpTrack
            --must:clean
            --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
            ${MPIEXEC_NUMPROC_FLAG} 2  $<TARGET_FILE:testOpTrack> 
            )
    
SET_TESTS_PROPERTIES (testOpTrack PROPERTIES PASS_REGULAR_EXPRESSION
    "MPI_PROD.*Operation created at.*MPI_Op_create.*MPI_OP_NULL")
 
