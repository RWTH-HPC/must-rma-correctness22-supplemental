# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for deadlock view tests that are part of deadlock detection.
#
# IMPORTANT: these tests are only semi-automatic, the format and appearence of the deadlock
#                     view is stressed here, which is not automatically checked.
#
# @author Tobias Hilbrich
# @date 17.10.2012
#

###################
#Add test cases
addProcessLocalTest ("DLViewCommmismatch" "DLViewCommmismatch.c"  2
    "The application issued a set of MPI calls that can cause a deadlock!."
    ""
    )

addProcessLocalTest ("DLViewTagmismatch" "DLViewTagmismatch.c"  2
    "The application issued a set of MPI calls that can cause a deadlock!."
    ""
    )

addProcessLocalTest ("DLViewMulticommMultitag" "DLViewMulticommMultitag.c"  2
    "The application issued a set of MPI calls that can cause a deadlock!."
    ""
    )

addProcessLocalTest ("DLViewMultiLocationInMsgQueue" "DLViewMultiLocationInMsgQueue.c"  2
    "The application issued a set of MPI calls that can cause a deadlock!."
    ""
    )
    