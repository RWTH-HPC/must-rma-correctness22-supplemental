# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause


#
# The PnMPI Callpath module
#

ADD_WRAPPED_FILE(wrappers.cpp wrappers.w)

INCLUDE_DIRECTORIES (${PnMPI_INCLUDE_PATH})

#compile callpath module
GTI_MAC_ADD_MODULE (callpathModule "callpath_module.cpp;wrappers.cpp" "CXX")
TARGET_LINK_LIBRARIES(callpathModule callpath)

MUST_INSTALL(FILES callpath_module.h DESTINATION include)

