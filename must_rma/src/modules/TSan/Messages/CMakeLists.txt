# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.txt
#       CMake file for modules of directory modules/TSan/Messages.
#
# @author Joachim Protze, Tobias Hilbrich, Felix Dommes
# @date 16.12.2017
#

#Install headers
MUST_INSTALL(FILES I_TSanMessages.h DESTINATION include)


#Build and install modules 
GTI_MAC_ADD_MODULE (TSanMessages "TSanMessages.cpp;${BASE_SOURCES}" "CXX")

add_library(onReportLoader STATIC OnReportLoader.cpp)
INSTALL(TARGETS
     onReportLoader
    DESTINATION 
        "${CMAKE_INSTALL_PREFIX}/lib"
    PERMISSIONS 
        OWNER_READ OWNER_WRITE 
        GROUP_READ 
        WORLD_READ
    )
