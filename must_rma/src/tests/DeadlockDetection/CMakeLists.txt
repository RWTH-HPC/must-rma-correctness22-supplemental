# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for deadlock detection tests.
#
# @author Tobias Hilbrich
# @date 26.07.2011
#

##################
#Recurse into sub directories
ADD_SUBDIRECTORY(P2PMatch)
ADD_SUBDIRECTORY(CollectiveMatch)
ADD_SUBDIRECTORY(DeadlockDetection)
ADD_SUBDIRECTORY(DeadlockView)
ADD_SUBDIRECTORY(DP2PMatch)
ADD_SUBDIRECTORY(DCollectiveMatch)

#Only go into NBC tests if we got all of them
IF (HAVE_MPI_IALLGATHER AND 
    HAVE_MPI_IALLGATHERV AND
    HAVE_MPI_IALLREDUCE AND
    HAVE_MPI_IALLTOALL AND 
    HAVE_MPI_IALLTOALLV AND 
    HAVE_MPI_IALLTOALLW AND
    HAVE_MPI_IBARRIER AND
    HAVE_MPI_IBCAST AND
    HAVE_MPI_IGATHER AND
    HAVE_MPI_IGATHERV AND
    HAVE_MPI_IREDUCE AND
    HAVE_MPI_IREDUCE_SCATTER AND
    HAVE_MPI_IREDUCE_SCATTER_BLOCK AND
    HAVE_MPI_ISCAN AND
    HAVE_MPI_IEXSCAN AND
    HAVE_MPI_ISCATTER AND
    HAVE_MPI_ISCATTERV)
    ADD_SUBDIRECTORY(NbcCollectiveMatch)
ENDIF ()