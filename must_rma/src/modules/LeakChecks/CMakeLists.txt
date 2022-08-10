# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for the modules/LeakChecks directory.
#
# @author Tobias Hilbrich
# @date 17.05.2011

#Install headers
MUST_INSTALL(
    FILES 
        I_LeakChecks.h
    DESTINATION 
        include)

#Build and install modules 
GTI_MAC_ADD_MODULE (leakChecks "LeakChecks.cpp;${GTI_SOURCE_PATH}/CompletionTree.cpp;${BASE_SOURCES}" "CXX")
