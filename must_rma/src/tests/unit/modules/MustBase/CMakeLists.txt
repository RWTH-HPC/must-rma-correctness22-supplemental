# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

add_library(PrefixedOstreamTest PrefixedOstreamTest.cpp)
target_link_libraries(PrefixedOstreamTest PRIVATE ${gtest})
target_include_directories(PrefixedOstreamTest PRIVATE ${CMAKE_SOURCE_DIR}/modules/MustBase)
target_sources(UnitTests PRIVATE $<TARGET_OBJECTS:PrefixedOstreamTest>)


add_library(MsgFilterTest ${CMAKE_SOURCE_DIR}/modules/MustBase/MsgFilter.cpp MsgFilterTest.cpp)
target_link_libraries(MsgFilterTest PRIVATE ${gtest})
target_include_directories(MsgFilterTest PUBLIC
    ${CMAKE_BINARY_DIR}/modules/Common
    ${CMAKE_SOURCE_DIR}/modules/Common)
target_include_directories(MsgFilterTest PRIVATE ${CMAKE_SOURCE_DIR}/modules/MustBase ${CMAKE_BINARY_DIR}/modules/MustBase)
target_sources(UnitTests PRIVATE $<TARGET_OBJECTS:MsgFilterTest>)
