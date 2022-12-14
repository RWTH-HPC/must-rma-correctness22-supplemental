# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for P2P matching tests that are part of deadlock detection.
#
# @author Tobias Hilbrich
# @date 26.07.2011
#

###################
#Add test case

## simpleNoLoss
addProcessLocalTest ("simpleNoLoss" "simpleNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## anyTagNoLoss
addProcessLocalTest ("anyTagNoLoss" "anyTagNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## nonBlockingNoLoss
addProcessLocalTest ("nonBlockingNoLoss" "nonBlockingNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## wcRecvNoLoss
addProcessLocalTest ("wcRecvNoLoss" "wcRecvNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## wcIrecvNoLoss
addProcessLocalTest ("wcIrecvNoLoss" "wcIrecvNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## wcIrecvExNoLoss
addProcessLocalTest ("wcIrecvExNoLoss" "wcIrecvExNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## wcIrecvEx2NoLoss
addProcessLocalTest ("wcIrecvEx2NoLoss" "wcIrecvEx2NoLoss.c"   4
    ""
    "Error;ERROR;Warning;WARNING"
    )


## wcIrecvEx2Loss
## This can both produce a deadlock error or a lost message error!!!
## lost message can come from rank 1-3
addProcessLocalTest ("wcIrecvEx2Loss" "wcIrecvEx2Loss.c"   4
    "Error.*MPI_Isend@.*Lost send of rank . to rank 0;MUST detected a deadlock"
    ""
    )


## commNoLoss
addProcessLocalTest ("commNoLoss" "commNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## interCommNoLoss
addProcessLocalTest ("interCommNoLoss" "interCommNoLoss.c"   4
    ""
    "Error;ERROR;Warning;WARNING"
    )


## sendRecvNoLoss
addProcessLocalTest ("sendRecvNoLoss" "sendRecvNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## startAllNoLoss
addProcessLocalTest ("startAllNoLoss" "startAllNoLoss.c"   2
    ""
    "Error;ERROR;Warning;WARNING"
    )


## complexNoLoss
addProcessLocalTest ("complexNoLoss" "complexNoLoss.c"  4
    ""
    "Error;ERROR;Warning;WARNING"
    )


 