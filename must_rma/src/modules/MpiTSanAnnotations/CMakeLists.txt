# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.txt
#       CMake file for modules of directory modules/XMP/MpiTSanAnnotations
#

#Install headers

IF (NOT ENABLE_TSAN)
    RETURN()
ENDIF ()

MUST_INSTALL(FILES 
            I_MpiTSanAnnotations.h 
        DESTINATION include)


#Build and install modules 
GTI_MAC_ADD_MODULE (mpiTSanAnnotations "../ResourceTracking/Datatype/StridedBlock.cpp;MpiTSanAnnotations.cpp;${BASE_SOURCES}" "CXX")
