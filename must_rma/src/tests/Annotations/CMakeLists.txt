# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for Annotation test.
#
# @author Felix Tomski
# @date 18.03.2022
#


###################
#Add test case

##IntegerNoError   (NO-ERROR)
addProcessLocalTest ("AnnotationWarningIfZero" "AnnotationWarningIfZero.cpp" 2 
    ""
    "Error;ERROR;Warning;WARNING"
    )
