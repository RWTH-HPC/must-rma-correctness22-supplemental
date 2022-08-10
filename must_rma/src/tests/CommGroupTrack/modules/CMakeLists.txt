# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for modules of MPI communicator and group tracking test.
#
# @author Tobias Hilbrich
# @date 04.03.2011
#

#Install headers
INSTALL(FILES I_PrintCommGroup.h DESTINATION include)

#Build and install modules 
GTI_MAC_ADD_MODULE (printCommGroup "PrintCommGroup.cpp;${BASE_SOURCES}" "CXX")

