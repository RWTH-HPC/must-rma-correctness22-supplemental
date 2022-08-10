# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for modules of directory modules/VectorClockWrapper.
#
# @author Felix Tomski
# @date 29.06.2021
#

#Install headers
MUST_INSTALL(FILES I_VectorClockWrapper.h DESTINATION include)

#Build and install modules 
GTI_MAC_ADD_MODULE (vectorClockWrapper "VectorClockWrapper.cpp;${BASE_SOURCES}" "CXX")
