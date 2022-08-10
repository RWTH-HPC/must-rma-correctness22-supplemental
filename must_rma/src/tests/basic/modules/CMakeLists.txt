# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for modules of basic test.
#
# @author Tobias Hilbrich
# @date 03.01.2011
#

#Install headers
INSTALL(FILES I_TestLocation.h I_TestLog.h DESTINATION include)

#Build and install modules 
GTI_MAC_ADD_MODULE (testLocation "TestLocation.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (testLog "TestLog.cpp;${BASE_SOURCES}" "CXX")
