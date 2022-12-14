# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for the modules/MustBase directory.
#
# @author Tobias Hilbrich
# @date 07.01.2011

IF (USE_CALLPATH)
    INCLUDE_DIRECTORIES (../Callpath/util ../Callpath/src ../Callpath/module ${CMAKE_BINARY_DIR}/modules/Callpath/src)
ENDIF()

IF (USE_BACKWARD)
    INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/externals/backward-cpp)
ENDIF()


#Install headers
MUST_INSTALL(
    FILES 
        BaseApi.h 
        BaseIds.h 
        I_LocationAnalysis.h
        LocationInfo.h 
        I_ParallelIdAnalysis.h
        I_InitParallelId.h
        I_InitLocationId.h
        I_ArgumentAnalysis.h 
        I_LocationReduction.h
        I_CreateMessage.h
        I_MessageFileReduction.h
        I_MessageLogger.h 
        I_MessageReduction.h
        I_MessageReproducer.h
        I_FinishNotify.h
        I_FinalizeNotify.h
        I_FinalizeReduction.h
        I_FinalizeMust.h
    DESTINATION 
        include)

#Wrapp file
ADD_WRAPPED_FILE(StatusWrapper.cpp StatusWrapper.w)

#Build and install modules
GTI_MAC_ADD_MODULE (parallelIdAnalysis "ParallelIdImpl.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (initParallelIdMpi "InitParallelIdMpi.cpp;${GTI_SOURCE_PATH}/GtiHelper.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (initParallelIdHybrid "InitParallelIdHybrid.cpp;${GTI_SOURCE_PATH}/GtiHelper.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (locationAnalysis "LocationImpl.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (locationReduction "LocationReduction.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (argumentAnalysis "ArgumentAnalysis.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (messageFileReduction "MessageFileReduction.cpp;${GTI_SOURCE_PATH}/CompletionTree.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (createMessage "CreateMessage.cpp;MsgFilter.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (msgLoggerDebugger "MsgLoggerDebugger.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (msgLoggerStdOut "MsgLoggerStdOut.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (msgLoggerHtml "MsgLoggerHtml.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (msgLoggerDdt "MsgLoggerDdt.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (msgLoggerScoreP "MsgLoggerScoreP.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (msgLoggerReproduce "MsgLoggerReproduce.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (messageReproducer "MessageReproducer.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (finalizeNotify "FinalizeNotify.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (finalizeReduction "FinalizeReduction.cpp;${GTI_SOURCE_PATH}/CompletionTree.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (finalizeMust "FinalizeMust.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (messageReduction "MessageReduction.cpp;MessageAnalysis.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (finishNotify "FinishNotify.cpp;${BASE_SOURCES}" "CXX")

GTI_MAC_ADD_MODULE (initLocationId "InitLocationId.cpp;${BASE_SOURCES}" "CXX")

# Link backward-cpp for stack trace analysis
IF (USE_BACKWARD)
  GTI_MAC_ADD_MODULE (initLocationIdBackward "InitLocationId.cpp;${BASE_SOURCES}" "CXX")
  target_compile_definitions(initLocationIdBackward PUBLIC BUILD_BACKWARD)
  TARGET_LINK_LIBRARIES(initLocationIdBackward Backward::Backward)
ENDIF()
IF (USE_CALLPATH)
  GTI_MAC_ADD_MODULE (initLocationIdCallpath "InitLocationId.cpp;${BASE_SOURCES}" "CXX")
  target_compile_definitions(initLocationIdCallpath PUBLIC BUILD_CALLPATH)
ENDIF()

IF (HAVE_MPI_STATUS_IGNORE OR HAVE_MPI_STATUSES_IGNORE)
	GTI_MAC_ADD_MODULE (statusWrapper "StatusWrapper.cpp" "CXX")
ENDIF ()

#Special properties, for DDT logger we must add "-g"
SET_SOURCE_FILES_PROPERTIES(MsgLoggerDdt.cpp PROPERTIES COMPILE_FLAGS -g)
