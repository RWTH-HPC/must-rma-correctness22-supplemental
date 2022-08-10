# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for DatatypeTrack test.
#
# @author Tobias Hilbrich
# @date 23.02.2011
#

##################
#Recurse into sub directories
ADD_SUBDIRECTORY (modules)



###################
#Add test case
addTestExecutable ("datatypeTrackTest" "datatype.c")
CONFIGURE_FILE(analysis_spec.xml ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml @ONLY)

configureTest (
    ${CMAKE_CURRENT_BINARY_DIR}/layout.xml
    "FinalizeNotify:MUST_Base"
    ""
    "PrintDatatype:TestDatatypeTrack;MsgLoggerHtml:MUST_Base;MsgLoggerStdOut:MUST_Base"
    2
    1
    )

IF(TEST_PREBUILDS)
    MUST_PREBUILD_CONFIGURATION(
	prebuild-testDatatypeTrack
	"--must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml \
             --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml \
             ${MPIEXEC_NUMPROC_FLAG} 2"
	)
ENDIF()

ADD_TEST(NAME testDatatypeTrack COMMAND
        bash ${MUSTRUN} 
            --must:timeout ${MUSTRUN_TIMEOUT}
            --must:mpiexec ${MPIEXEC} 
            --must:np ${MPIEXEC_NUMPROC_FLAG} 
            --must:layout ${CMAKE_CURRENT_BINARY_DIR}/layout.xml 
            --must:temp tempDatatypeTrack 
            --must:clean
            --must:analyses ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
            ${MPIEXEC_NUMPROC_FLAG} 2  $<TARGET_FILE:datatypeTrackTest> 
            )
    
SET_TESTS_PROPERTIES (testDatatypeTrack PROPERTIES PASS_REGULAR_EXPRESSION 
    "Datatype created at reference 1 is for C, based on the following type.*MPI_Type_struct")

##Example_4_2   (Example 4.6 from MPI-2.2 standard)
addProcessLocalTestWithAnalysis ("Example_4_2" "example_4_2.c" 1 
    "PrintDatatype:TestDatatypeTrack;MsgLoggerHtml:MUST_Base"
    ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
    "Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}.*Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}"
    ""
    )
##Example_4_3   (Example 4.3 from MPI-2.2 standard)
addProcessLocalTestWithAnalysis ("Example_4_3" "example_4_3.c" 1 
    "PrintDatatype:TestDatatypeTrack;MsgLoggerHtml:MUST_Base"
    ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
    "Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}.*Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\).*\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}"
    ""
    )
##Example_4_4   (Example 4.4 from MPI-2.2 standard)
addProcessLocalTestWithAnalysis ("Example_4_4" "example_4_4.c" 1 
    "PrintDatatype:TestDatatypeTrack;MsgLoggerHtml:MUST_Base"
    ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
    "Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}.*Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_DOUBLE, -[0-9]*\\\\), \\\\(MPI_CHAR, -[0-9]*\\\\), \\\\(MPI_DOUBLE, -[0-9]*\\\\), \\\\(MPI_CHAR, -[0-9]*\\\\)}"
    ""
    )
##Example_4_5   (Example 4.5 from MPI-2.2 standard)
addProcessLocalTestWithAnalysis ("Example_4_5" "example_4_5.c" 1 
    "PrintDatatype:TestDatatypeTrack;MsgLoggerHtml:MUST_Base"
    ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
    "Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}.*Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}"
    ""
    )
##Example_4_6   (Example 4.6 from MPI-2.2 standard)
addProcessLocalTestWithAnalysis ("Example_4_6" "example_4_6.c" 1 
    "PrintDatatype:TestDatatypeTrack;MsgLoggerHtml:MUST_Base"
    ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
    "Typemap = {\\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}.*Typemap = {\\\\(MPI_FLOAT, [0-9]*\\\\), \\\\(MPI_FLOAT, [0-9]*\\\\), \\\\(MPI_DOUBLE, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\), \\\\(MPI_CHAR, [0-9]*\\\\)}"
    ""
    )
##Example_4_9   (Example 4.9 from MPI-2.2 standard)
addProcessLocalTestWithAnalysis ("Example_4_9" "example_4_9.c" 1 
    "PrintDatatype:TestDatatypeTrack;MsgLoggerHtml:MUST_Base"
    ${CMAKE_CURRENT_BINARY_DIR}/analysis_spec.xml
    "Typemap = {\\\\(MPI_LB, -[0-9]*\\\\), \\\\(MPI_INT, [0-9]*\\\\), \\\\(MPI_UB, [0-9]*\\\\)}.*Typemap = {\\\\(MPI_LB, -[0-9]*\\\\), \\\\(MPI_INT, [0-9]*\\\\), \\\\(MPI_INT, [0-9]*\\\\), \\\\(MPI_UB, [0-9]*\\\\)}"
    ""
    )
