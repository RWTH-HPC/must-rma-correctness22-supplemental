# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.txt
#       CMake file for modules of directory modules/XMP/MpiTypeArt
#

IF (NOT ENABLE_TYPEART)
    RETURN()
ENDIF ()

#Install headers
MUST_INSTALL(FILES I_MpiTypeArt.h DESTINATION include)


#Build and install modules 
GTI_MAC_ADD_MODULE (mpiTypeArt "../ResourceTracking/Datatype/StridedBlock.cpp;MpiTypeArt.cpp;${BASE_SOURCES}" "CXX")

target_link_libraries(mpiTypeArt typeart::Runtime)
