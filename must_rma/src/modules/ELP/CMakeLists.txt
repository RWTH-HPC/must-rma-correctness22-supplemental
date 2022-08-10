# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for modules of directory modules/BasicChecks.
#
# @author Mathias Korepkat, Tobias Hilbrich
# @date 02.03.2011
#
#
# TODO: Rename into something more clear at some future point (e.g. OpenMP)

IF (NOT OPENMP_FOUND)
    RETURN()
ENDIF ()


#Install headers
MUST_INSTALL(
    FILES 
        I_ThreadSanity.h 
        I_ELPSubModule.h
        I_OMPTIntegration.h
    DESTINATION include)

#Add extra includes and library dependencies for OMPT and libunwind
if (GTI_OMPT_FOUND)
    include_directories(${GTI_OMPT_INCLUDE_PATH})
endif (GTI_OMPT_FOUND)

if (GTI_FOUND_LIBUNWIND)
    include_directories(${GTI_LIBUNWIND_INCLUDE_PATH})
endif (GTI_FOUND_LIBUNWIND)

##Modules need omp.h, not every compiler supports that
IF (OPENMP_FOUND) 
    #Build and install modules
    GTI_MAC_ADD_MODULE (threadSanity "ThreadSanity.cpp;${BASE_SOURCES}" "CXX")
    #GTI_MAC_ADD_MODULE (elpSubModule "ELPSubModule.cpp;${BASE_SOURCES}" "CXX")
    # Build it for now, even if GTI_OMPT_FOUND is not set
    # it will be empty then
    GTI_MAC_ADD_MODULE(OMPTStartup "OMPTStartup.cpp" "CXX")

    # Add the libunwind dependency explicitly here
    if (GTI_FOUND_LIBUNWIND)
        target_link_libraries(threadSanity
${GTI_LIBUNWIND_LIB_PATH}/libunwind.so)
    endif (GTI_FOUND_LIBUNWIND)

    # Add the ompt dependency explicitly here
    #if (GTI_OMPT_FOUND)
    #    target_link_libraries(threadSanity ${GTI_OMPT_LIB_PATH}/libiomp5.so)
    #endif (GTI_OMPT_FOUND)
ENDIF (OPENMP_FOUND)
