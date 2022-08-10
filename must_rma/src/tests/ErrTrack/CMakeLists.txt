# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for errorhandler tracking test.
#
# @author Tobias Hilbrich
# @date 13.05.2011
#

##################
#Recurse into sub directories
ADD_SUBDIRECTORY (modules)

###################
#Add test case
addTestExecutable ("testErrTrack" "testErrTrack.c")
CONFIGURE_FILE(analysis_spec.xml ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml @ONLY)

configureTest (
    ${CMAKE_CURRENT_BINARY_DIR}/layout.xml
    "FinalizeNotify:MUST_Base"
    "PrintErr:TestErrTrack"
    "MsgLoggerHtml:MUST_Base;MsgLoggerStdOut:MUST_Base"
    2
    1
    )

IF(TEST_PREBUILDS)
    MUST_PREBUILD_CONFIGURATION(
	prebuild-testErrTrack
	"--must:layout ${CMAKE_CURRENT_BINARY_DIR}/${test}layout.xml \
         --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml \
             ${MPIEXEC_NUMPROC_FLAG} 2"
	)
ENDIF()

ADD_TEST(NAME testErrTrack COMMAND
        bash ${MUSTRUN} 
            --must:timeout ${MUSTRUN_TIMEOUT}
            --must:mpiexec ${MPIEXEC} 
            --must:np ${MPIEXEC_NUMPROC_FLAG} 
            --must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml 
            --must:temp tempErrTrack
            --must:clean
            --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
            ${MPIEXEC_NUMPROC_FLAG} 2  $<TARGET_FILE:testErrTrack> 
            )
    
SET_TESTS_PROPERTIES (testErrTrack PROPERTIES PASS_REGULAR_EXPRESSION
    "MPI_ERRORS_ARE_FATAL.*MPI_ERRORS_RETURN.*Error handler.*MPI_ERRHANDLER_NULL")
 
