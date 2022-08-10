# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for modules of directory modules/CommChecks.
#
# @author Mathias Korepkat, Tobias Hilbrich
# @date 14.04.2011
#

#Install headers
MUST_INSTALL(FILES I_CommChecks.h DESTINATION include)

#Build and install modules 
GTI_MAC_ADD_MODULE (commChecks "CommChecks.cpp;${BASE_SOURCES}" "CXX")
