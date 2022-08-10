# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause


GTI_MAC_ADD_MODULE(MpiADadapter  "adapter.cpp" "CXX")


MUST_INSTALL(
    FILES
        I_MpiADadapter.h
    DESTINATION
        include)
