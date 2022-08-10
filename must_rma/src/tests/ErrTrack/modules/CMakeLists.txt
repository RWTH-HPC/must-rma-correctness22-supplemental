# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for modules of errorhandler track test.
#
# @author Tobias Hilbrich
# @date 13.01.2011
#

#Install headers
INSTALL(FILES I_PrintErr.h DESTINATION include)

#Build and install modules 
GTI_MAC_ADD_MODULE (printErr "PrintErr.cpp;${BASE_SOURCES}" "CXX")

