# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.txt
#       CMake file for modules of directory modules/OneSidedChecks.
#
# @author Simon Schwitanski 
# @date 22.05.2017
#

INCLUDE_DIRECTORIES(BEFORE AppThrAnn Common RMAOp RMATrack OriginChecks MPIVectorClock)

#Install headers
MUST_INSTALL(FILES
    RMATrack/OneSidedChecksApi.h
    Common/DataPool.h
    MPIVectorClock/I_MPIVectorClock.h
    AppThrAnn/I_AppThrAnn.h
    AppThrAnn/AppThrAnn.h
    AppThrAnn/AppThrAnn.cpp
    AppThrAnn/I_TSanSyncClockRecorder.h
    AppThrAnn/TSanSyncClockRecorder.h
    RMATrack/I_RMATrack.h
    RMATrack/RMATrack.h
    RMATrack/RMAMap.h
    RMATrack/RMAMap.hpp
    RMATrack/OriginRMAMap.h
    RMATrack/TargetRMAMap.h
    RMAOp/I_RMAOp.h
    RMAOp/RMAOp.h
    RMAOp/I_OriginRMAOp.h
    RMAOp/OriginRMAOp.h
    RMAOp/I_TargetRMAOp.h
    RMAOp/TargetRMAOp.h
    OriginChecks/I_OriginChecks.h
    TargetChecks/I_TargetChecks.h
    ../ResourceTracking/Utility/HandleInfoBase.h
    DESTINATION include)

#Build and install modules 
GTI_MAC_ADD_MODULE (mPIVectorClock "MPIVectorClock/MPIVectorClock.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (appThrAnn "AppThrAnn/AppThrAnn.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (tSanSyncClockRecorder "AppThrAnn/TSanSyncClockRecorder.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (rMATrack "../ResourceTracking/Datatype/StridedBlock.cpp;../ResourceTracking/Utility/HandleInfoBase.cpp;RMATrack/OriginRMAMap.cpp;RMAOp/RMAOp.cpp;RMAOp/OriginRMAOp.cpp;RMAOp/TargetRMAOp.cpp;RMATrack/RMATrack.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (originChecks "OriginChecks/OriginChecks.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (targetChecks "TargetChecks/TargetChecks.cpp;${BASE_SOURCES}" "CXX")
