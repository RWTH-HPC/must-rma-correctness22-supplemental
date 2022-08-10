# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause
#
# @file CMakeLists.txt
#       CMake file for modules of directory modules/DatatypeChecks.
#
# @author Joachim Protze, Tobias Hilbrich
# @date 23.05.2011
#

#Install headers
MUST_INSTALL(FILES I_BufferChecks.h DESTINATION include)


#Build and install modules 
GTI_MAC_ADD_MODULE (bufferChecks "BufferChecks.cpp;${BASE_SOURCES}" "CXX")
