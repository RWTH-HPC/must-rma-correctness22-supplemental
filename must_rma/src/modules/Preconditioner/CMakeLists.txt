# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.txt
#       CMake file for modules of directory modules/Preconditioner.
#
# @author Joachim Protze, Tobias Hilbrich
# @date 06.06.2011
#

#Install headers
MUST_INSTALL(FILES 
            I_RequestCondition.h 
            RequestConditionApi.h
            I_CollectiveCondition.h 
            CollectiveConditionApi.h
            I_OnlyOnRootCondition.h 
            OnlyOnRootConditionApi.h
            I_CompletionCondition.h 
            CompletionConditionApi.h
        DESTINATION include)


#Build and install modules 
GTI_MAC_ADD_MODULE (requestCondition "RequestCondition.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (collectiveCondition "CollectiveCondition.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (onlyOnRootCondition "OnlyOnRootCondition.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (completionCondition "CompletionCondition.cpp;${BASE_SOURCES}" "CXX")