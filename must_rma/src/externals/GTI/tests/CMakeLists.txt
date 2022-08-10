# This file is part of GTI (Generic Tool Infrastructure)
#
# Copyright (C)
#   2008-2019 ZIH, Technische Universitaet Dresden, Federal Republic of Germany
#   2008-2019 Lawrence Livermore National Laboratories, United States of America
#   2013-2019 RWTH Aachen University, Federal Republic of Germany
#
# See the LICENSE file in the package base directory for details

##
# @file CMakeLists.cmake
#       GTI CMake file for tests.
#
# @author Tobias Hilbrich
# @date 06.04.2009
#
# The tests have to be used as follows:
#    ccmake       #configure with CMake, enable the test option there, generate
#    make         #build
#    make install #install and patch the libraries
#    make test    #run the tests
#
# Currently likely not very portable, should work well with OpenMPI atm.
#

# Allow use of LOCATION property for build targets (deprecated, should be fixed soon)
IF (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.0")
    CMAKE_POLICY(SET CMP0026 OLD)
ENDIF()

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/modules/utility ${PROJECT_SOURCE_DIR}/modules/common ${PROJECT_BINARY_DIR}/modules/common ${PROJECT_SOURCE_DIR}/modules/interfaces ${PnMPI_INCLUDE_PATH} ${MPI_C_INCLUDE_PATH}) #P^nMPI inc dir needed for modules!

##################
## Helper function that generates tests with mpiexec commands
FUNCTION (ADD_MPIEXEC_TEST TEST_NAME NUM_PROCS EXECUTABLE)
FOREACH(arg IN LISTS ARGN)
    LIST(APPEND ENV_VARIABLES "${arg}")
ENDFOREACH()
ADD_TEST(${TEST_NAME} ${MPIEXEC_EXECUTABLE}
         ${MPIEXEC_NUMPROC_FLAG} ${NUM_PROCS}
         env ${ENV_VARIABLES} "${EXECUTABLE}")
ENDFUNCTION()


##################
#Debug on/off
#SET (CMAKE_VERBOSE_MAKEFILE ON FORCE)
ADD_SUBDIRECTORY (Simple)
ADD_SUBDIRECTORY (reduction)
ADD_SUBDIRECTORY (HelloWeaver)

###################
#Build Testing modules ....

SET (BASE_SOURCES)

GTI_MAC_ADD_MODULE (test_cstrat_t "test_cstrat_t.cxx;../modules/utility/GtiHelper.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (test_cstrat_b "test_cstrat_b.cxx;../modules/utility/GtiHelper.cpp;${BASE_SOURCES}" "CXX")

###################
#Set libs, dirs, ...
IF (DEBUG_BUILD)
    SET(CMAKE_VERBOSE_MAKEFILE TRUE)
ENDIF (DEBUG_BUILD)
SET(CMAKE_C_FLAGS ${MPI_C_COMPILE_FLAGS})
INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH})

FOREACH (lan C CXX Fortran)
    SET (${lan}_compiler_to_use ${CMAKE_${lan}_COMPILER})
    IF (MPI_${lan}_COMPILER)
        SET (${lan}_compiler_to_use ${MPI_${lan}_COMPILER})
    ENDIF ()
ENDFOREACH ()

SET(CMAKE_C_COMPILER ${C_compiler_to_use})
SET(CMAKE_CXX_COMPILER ${CXX__compiler_to_use})
SET(CMAKE_EXE_LINKER_FLAGS ${MPI_C_LINK_FLAGS})#For the tests we have to link against P^nMPI lib
#LINK_DIRECTORIES("${PnMPI_LIBRARY_DIR}")
SET (TEST_LIBRARIES ${PnMPI_C_LIBRARIES}) 


###################
#Tests (C)

##Simple test for ChannelId implementation
ADD_EXECUTABLE (TestChannelId TestChannelId.cpp)
ADD_TEST(TestChannelId ${EXECUTABLE_OUTPUT_PATH}/TestChannelId)
SET (passRegex 
    "0.1048574.0.1048574.1024.*1048574.0.1048574.1024.1048574")
SET_TESTS_PROPERTIES (TestChannelId 
    PROPERTIES PASS_REGULAR_EXPRESSION "${passRegex}")

## Hello World with some modules
ADD_EXECUTABLE (hello_pnmpi hello.c)
TARGET_LINK_LIBRARIES(hello_pnmpi ${TEST_LIBRARIES})

ADD_MPIEXEC_TEST(hello_pnmpi 2 ${EXECUTABLE_OUTPUT_PATH}/hello_pnmpi
            PNMPI_LIB_PATH=${CMAKE_INSTALL_PREFIX}/modules
            LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/modules:${PnMPI_LIBRARY_DIR}:$LD_LIBRARY_PATH
            DYLD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/modules:${PnMPI_LIBRARY_DIR}:$DYLD_LIBRARY_PATH
            PNMPI_CONF=${PROJECT_SOURCE_DIR}/tests/hello-pnmpi.conf
            )

SET_TESTS_PROPERTIES(hello_pnmpi PROPERTIES
    FAIL_REGULAR_EXPRESSION "No modules loaded" # Check if loading modules was successful
)

## Test for TCP comm protocol with simple comm strat
#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/tests/test_comm_simple_tcp.sh.in ${PROJECT_BINARY_DIR}/bin/test_comm_simple_tcp.sh)
#ADD_TEST(cstrat_simple_up_cprot_simple_tcp 
#    bash ${PROJECT_BINARY_DIR}/bin/test_comm_simple_tcp.sh)
#SET (passRegex "Hallo1.*Hallo2.*Hallo3" "Hallo1.*Hallo3.*Hallo2")
#SET_TESTS_PROPERTIES (cstrat_simple_up_cprot_simple_tcp 
#    PROPERTIES PASS_REGULAR_EXPRESSION "${passRegex}")
    
## Test for MPI Comm Protocol with splitted MPI_COMM_WORLD
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/tests/test_comm_mpi_split.sh.in ${PROJECT_BINARY_DIR}/bin/test_comm_mpi_split.sh)
ADD_TEST(test_cprot_splitted_mpi 
    bash ${PROJECT_BINARY_DIR}/bin/test_comm_mpi_split.sh)
SET (passRegex "Hallo1.*Hallo2.*Hallo3" "Hallo1.*Hallo3.*Hallo2")
SET_TESTS_PROPERTIES (test_cprot_splitted_mpi 
    PROPERTIES PASS_REGULAR_EXPRESSION "${passRegex}")

## Test for record generation interface and its GTI implementation
#TODO geting interface implementation uses P^nMPI, which requires an mpirun
ADD_EXECUTABLE (test_record_gen test_wrappgen.cpp)
TARGET_LINK_LIBRARIES(test_record_gen ${TEST_LIBRARIES})
ADD_MPIEXEC_TEST(test_record_gen 1 ${EXECUTABLE_OUTPUT_PATH}/test_record_gen
            GTI_RECORD_GEN_IMPL=${CMAKE_INSTALL_PREFIX}/modules/libgtiRecordGenImpl.so
            LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/modules:${PnMPI_LIBRARY_DIR}:$LD_LIBRARY_PATH
            DYLD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/modules:${PnMPI_LIBRARY_DIR}:$DYLD_LIBRARY_PATH
            )
        
## Exhaustive test for record generation interface and its GTI implementation
#TODO geting interface implementation uses P^nMPI, which requires an mpirun
ADD_EXECUTABLE (test_record_gen_exhaustive test_wrappgen_exhaustive.cpp)
TARGET_LINK_LIBRARIES(test_record_gen_exhaustive ${TEST_LIBRARIES})
ADD_MPIEXEC_TEST(test_record_gen_exhaustive 1 ${EXECUTABLE_OUTPUT_PATH}/test_record_gen_exhaustive
            GTI_RECORD_GEN_IMPL=${CMAKE_INSTALL_PREFIX}/modules/libgtiRecordGenImpl.so
            LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/modules:${PnMPI_LIBRARY_DIR}:$LD_LIBRARY_PATH
            DYLD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/modules:${PnMPI_LIBRARY_DIR}:$DYLD_LIBRARY_PATH
            )
        
###################
#Weaver tests
GET_TARGET_PROPERTY(WEAVER_EXE weaver LOCATION)
ADD_TEST(weaver_test 
    ${WEAVER_EXE} 
        ${PROJECT_SOURCE_DIR}/tests/example-layout-specification.xml
        ${CMAKE_INSTALL_PREFIX}/specifications/gti-specification.xml 
        ${PROJECT_SOURCE_DIR}/tests/example-api-specification.xml
        ${PROJECT_SOURCE_DIR}/tests/example-analysis-specification.xml
        ${PROJECT_SOURCE_DIR}/tests/example-analysis-specification2.xml
        )
        
#weaver simple test
ADD_EXECUTABLE (test_driver_weaver_simple SimpleTest.cpp)
TARGET_LINK_LIBRARIES(test_driver_weaver_simple ${TEST_LIBRARIES})
GET_TARGET_PROPERTY(WRAPPGEN_EXE wrappgen LOCATION)
GET_TARGET_PROPERTY(BUILDGEN_EXE buildgen LOCATION)
GET_TARGET_PROPERTY(MODCONFGEN_EXE modconfgen LOCATION)
GET_TARGET_PROPERTY(WEAVER_TEST_DRIVER_EXE test_driver_weaver_simple LOCATION)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/tests/simple-case-analysis-specification.xml.in ${PROJECT_BINARY_DIR}/tests/simple-case-analysis-specification.xml @ONLY)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/tests/simple-case-api-specification.xml.in ${PROJECT_BINARY_DIR}/tests/simple-case-api-specification.xml @ONLY)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/tests/SimpleWeaverTest.sh.in ${PROJECT_BINARY_DIR}/tests/SimpleWeaverTest.sh)

ADD_TEST(weaver_simple_test 
    bash ${PROJECT_BINARY_DIR}/tests/SimpleWeaverTest.sh
    )
SET_TESTS_PROPERTIES(weaver_simple_test PROPERTIES
    FAIL_REGULAR_EXPRESSION "Error"
)
