# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for basic test.
#
# @author Tobias Hilbrich
# @date 03.01.2011
#

##################
#Recurse into sub directories
ADD_SUBDIRECTORY (modules)

###################
#Add test case
addTestExecutable ("basic" "basic.c")
CONFIGURE_FILE(analysis_spec.xml ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml @ONLY)

configureTest (
    ${CMAKE_CURRENT_BINARY_DIR}/layout.xml
    "TestLog:Basic;FinalizeNotify:MUST_Base"
    ""
    "TestLocation:Basic;MsgLoggerStdOut:MUST_Base;MsgLoggerHtml:MUST_Base"
    4
    2
    )

IF(TEST_PREBUILDS)
    MUST_PREBUILD_CONFIGURATION(
	prebuild-basic
	"--must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml \
         --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml \
         ${MPIEXEC_NUMPROC_FLAG} 4"
	)
ENDIF()

ADD_TEST(NAME basic COMMAND
    bash ${MUSTRUN} 
        --must:timeout ${MUSTRUN_TIMEOUT}
        --must:mpiexec ${MPIEXEC} 
        --must:np ${MPIEXEC_NUMPROC_FLAG} 
        --must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml 
        --must:temp tempBasic 
        --must:clean
        --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
        ${MPIEXEC_NUMPROC_FLAG} 4  $<TARGET_FILE:basic> 
    )
    
SET_TESTS_PROPERTIES (basic PROPERTIES PASS_REGULAR_EXPRESSION "Test log message!.*MPI_Init.*rank")


addProcessLocalTest ("basic_initialized"
    "initialized.c"
    2
    ""
    "Error;ERROR;Warning;WARNING"
)
