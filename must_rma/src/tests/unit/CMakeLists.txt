# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# The following imported googletest targets are deprecated since
# CMake 3.20 and will be renamed in future.
set(gtest GTest::GTest)
set(gtest_main GTest::Main)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ".")

add_executable(UnitTests) # sources are added in the test directories
target_link_libraries(UnitTests ${gtest_main} ${gtest})

add_subdirectory(modules)

add_dependencies(check UnitTests)
add_dependencies(check-unit UnitTests)

configure_file(lit.cfg lit.cfg COPYONLY)
