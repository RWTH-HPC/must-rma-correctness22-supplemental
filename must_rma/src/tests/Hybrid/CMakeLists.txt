# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for CommChecks test.
#
# @author Tobias Hilbrich,Mathias Korepkat
# @date 14.04.2011
#

IF (NOT ENABLE_ELP_EXTENSIONS)
    RETURN()
ENDIF ()

IF (NOT (GTI_IS_THREAD_SAFE AND OpenMP_FOUND))
    RETURN()
ENDIF ()


##################
#Recurse into sub directories
##Not needed

###################
#Add test cases

find_package(OpenMP)

##IntegerGreaterCommSizeNoError   (NO-ERROR)
addHybridTest ("CommSizeOnWorkerThread" "CommSizeOnWorkerThread.cpp" 4 
    ""
    "Error;ERROR;Warning;WARNING"
    ""
    ""
    )
    
##MpiInitInParallel   (NO-ERROR)
addHybridTest ("MpiInitInParallel" "MpiInitInParallel.cpp" 4 
    ""
    "Error;ERROR;Warning;WARNING"
    ""
    ""
    )
    
