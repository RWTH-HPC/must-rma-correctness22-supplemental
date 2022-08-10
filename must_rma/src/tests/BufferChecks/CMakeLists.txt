# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for RequestChecks test.
#
# @author Tobias Hilbrich,Joachim Protze
# @date 11.01.2013
#

##################
#Recurse into sub directories
##Not needed

###################
#Add test cases

##RequestNoError   (NO-ERROR)
# addProcessLocalTest ("DatatypeNoError" "DatatypeNoError.cpp" 1 
#     ""
# #     "Error;ERROR;Warning;WARNING"
#     )

#BufferNoError   (NO-ERROR)
addProcessLocalTest ("BufferNoError" "BufferNoError.cpp" 2 
    ""
    "Error;ERROR;Warning;WARNING"
    )

#BufferDetachNotAttachedError   (ERROR)
addProcessLocalTest ("BufferDetachNotAttachedError" "BufferDetachNotAttachedError.cpp" 2 
    "Error: from: call MPI_Buffer_detach@.*: There was no buffer attached previously"
    ""
    )

#BufferedSendNotAttachedError   (ERROR)
addProcessLocalTest ("BufferedSendNotAttachedError" "BufferedSendNotAttachedError.cpp" 2 
    "Error: from: call MPI_Bsend@.*: There is no MPI buffer attached"
    ""
    )

#BufferOutsizeError   (ERROR)
addProcessLocalTest ("BufferOutsizeError" "BufferOutsizeError.cpp" 2 
    "Warning: from: call MPI_Bsend@.*: Latest buffer usage of 4000128 bytes outsized the attached buffer's size of 4000128 bytes while 4000128 bytes are already in use!"
    ""
    )

#BufferReattachError   (ERROR)
addProcessLocalTest ("BufferReattachError" "BufferReattachError.cpp" 2 
    "Error: from: call MPI_Buffer_attach@.*: There was already a buffer attached."
    ""
    )

#BufferReuseNoError   (NO-ERROR)
addProcessLocalTest ("BufferReuseNoError" "BufferReuseNoError.cpp" 2 
    ""
    "Error;ERROR;Warning;WARNING"
    )
