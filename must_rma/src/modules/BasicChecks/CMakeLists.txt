# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause
#
# @file CMakeLists.cmake
#       CMake file for modules of directory modules/BasicChecks.
#
# @author Mathias Korepkat, Tobias Hilbrich
# @date 02.03.2011
#

#Install headers
MUST_INSTALL(
    FILES 
        I_IntegerChecks.h 
        I_BasicIntegrities.h
        I_OperationChecks.h
    DESTINATION include)

#Build and install modules 
GTI_MAC_ADD_MODULE (integerChecks "IntegerChecks.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (basicIntegrities "BasicIntegrities.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (operationChecks "OperationChecks.cpp;${BASE_SOURCES}" "CXX")
