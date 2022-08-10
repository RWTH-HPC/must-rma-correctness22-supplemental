# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause
# @file CMakeLists.cmake
#       CMake file for the modules/TSan directory.
#
# @author Simon Schwitanski
# @date 18.05.2017
# @author Joachim Protze, Felix Dommes
# @date 16.12.2017

IF (NOT ENABLE_TSAN)
    RETURN()
ENDIF ()

#Install headers
MUST_INSTALL(FILES I_TSan.h DESTINATION include)

#Build and install modules 
GTI_MAC_ADD_MODULE (tSan "TSan.cpp;${BASE_SOURCES}" "CXX")

ADD_SUBDIRECTORY (Messages)
