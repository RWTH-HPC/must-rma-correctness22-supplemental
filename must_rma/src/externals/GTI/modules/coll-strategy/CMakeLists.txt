# This file is part of GTI (Generic Tool Infrastructure)
#
# Copyright (C)
#   2008-2019 ZIH, Technische Universitaet Dresden, Federal Republic of Germany
#   2008-2019 Lawrence Livermore National Laboratories, United States of America
#   2013-2019 RWTH Aachen University, Federal Republic of Germany
#
# See the LICENSE file in the package base directory for details

##
# @file CMakeLists.cmake
#       MUST file for the modules/coll-strategy directory.
#
# @author Felix Tomski
# @date 11.09.2021
#
SET (BASE_SOURCES)
INSTALL(FILES 
        "CollStratApi.h"
    DESTINATION 
        "${CMAKE_INSTALL_PREFIX}/include"
    PERMISSIONS 
        OWNER_READ OWNER_WRITE 
        GROUP_READ 
        WORLD_READ
    )

GTI_MAC_ADD_MODULE (collStratNaive "CollStratNaive.cpp;GtiCollStrat.h;../utility/GtiObject.h;../utility/Bitset.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (collStratBinomial "CollStratBinomial.cpp;../utility/BinomialTree.cpp;GtiCollStrat.h;../utility/GtiObject.h;../utility/Bitset.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (collStratMpi "CollStratMpi.cpp;${BASE_SOURCES}" "CXX")
GTI_MAC_ADD_MODULE (collStratTemplate "CollStratTemplate.cpp;${BASE_SOURCES}" "CXX")
