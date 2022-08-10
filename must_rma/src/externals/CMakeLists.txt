# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

INCLUDE(FindPackageMessage)


# Check that the externals directory is not included via 'ADD_SUBDIRECTORY()'.
#
# Some of MUST's dependencies (like GTI and PnMPI) provide their resources as
# IMPORTED targets. As these are not available in parent scope and the
# dependencies can't be searched multiple times because of their fallbacks from
# the submodules, this CMake file needs to be included in the project's root.
IF (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    MESSAGE(FATAL_ERROR "The 'externals' directory MUST NOT be included via "
                        "ADD_SUBDIRECTORY but by including its CMakeLists.txt "
                        "via INCLUDE().")
ENDIF()

SET(SOURCE_DIR "${PROJECT_SOURCE_DIR}/externals")
SET(BINARY_DIR "${PROJECT_BINARY_DIR}/externals")


# Search for GTI.
#
# By default, an existing and installed version of GTI will be used for MUST.
# However, if GTI is not installed at this system, the GTI sources will be
# included as submodule.
#
# NOTE: The status of finding GTI in the system will be stored in a variable to
#       be used for deciding, if prebuilds can be build. These are not
#       compatible with building GTI from source right now.
#
# NOTE: If GTI will be build from sources, future calls of configure will NOT
#       search for an installed version of GTI to avoid a previously build
#       version in MUST's install prefix to be used.
IF (NOT DEFINED GTI_IS_SUBPROJECT)
    FIND_PACKAGE(GTI 1.6 QUIET
        HINTS
            "${GTI_INSTALL_PREFIX}"
        )
ENDIF ()
IF (GTI_FOUND)
    # As FIND_PACKAGE() needs to be called with the 'QUIET' flag to hide errors
    # about not found packages, we'll have to print the success message now.
    FIND_PACKAGE_MESSAGE(GTI "Found GTI: ${GTI_VERSION}"
                             "system.${GTI_VERSION}")
ELSE ()
    # If GTI is not installed at the system and its sources are not available in
    # the subdirectory, the user either needs to drop the PnMPI sources into
    # this subdirectory, or (if GTI sources are fetched from git) checkout the
    # git submodules.
    IF (NOT EXISTS ${SOURCE_DIR}/GTI/CMakeLists.txt)
      MESSAGE(FATAL_ERROR "Could NOT find GTI or its sources. Try updating the "
                          "git submodules or drop the sources of GTI into "
                          "${SOURCE_DIR}/GTI")
    ENDIF ()

    # Add GTI as subproject. As GTI's CMake configuration exports its full
    # configuration, no further steps are needed to use GTI.
    ADD_SUBDIRECTORY(${SOURCE_DIR}/GTI)
    SET(GTI_IS_SUBPROJECT TRUE CACHE BOOL "Wheter GTI sources will be used")
    MARK_AS_ADVANCED(GTI_IS_SUBPROJECT)
ENDIF ()


# Search for wrap.
#
# By default, an existing and installed version of wrap will be used for MUST.
# However, if wrap is not installed at this system, the wrap sources will be
# included as submodule.
#
# NOTE: If wrap has already been included by the GTI sources, it MUST NOT be
#       included a second time to avoid "duplicate target" errors by CMake.
FIND_PACKAGE(wrap QUIET)
IF (wrap_FOUND)
    # As FIND_PACKAGE() needs to be called with the 'QUIET' flag to hide errors
    # about not found packages, we'll have to print the success message now.
    FIND_PACKAGE_MESSAGE(wrap "Found wrap: ${Wrap_EXECUTABLE}"
                              "system.${Wrap_EXECUTABLE}")
ELSE ()
    # If wrap is not installed at the system and its sources are not available
    # in the subdirectory, the user either needs to drop the wrap sources into
    # this subdirectory, or (if GTI sources are fetched from git) checkout the
    # git submodules.
    IF (NOT EXISTS ${SOURCE_DIR}/wrap/CMakeLists.txt)
        MESSAGE(FATAL_ERROR "Could NOT find wrap or its sources. Try updating "
                            "the git submodules or drop the sources of wrap "
                            "into ${SOURCE_DIR}/wrap")
    ENDIF ()

    # Add wrap as submodule and print a message with its path to CMake's
    # console. This message will be printed only once.
    ADD_SUBDIRECTORY(${SOURCE_DIR}/wrap EXCLUDE_FROM_ALL)
    FIND_PACKAGE_MESSAGE(wrap
        "Found wrap: ${SOURCE_DIR}/wrap/wrap.py"
        "submodule.${SOURCE_DIR}/wrap/wrap.py")

    # Initialize the wrap configuration and include the generated configuration
    # file. This includes the required 'add_wrapped_file()' function.
    SET(WRAP ${SOURCE_DIR}/wrap/wrap.py)
    SET(GETSTATUSSIZE ${SOURCE_DIR}/wrap/getStatusSize.f)
    CONFIGURE_FILE(${SOURCE_DIR}/wrap/wrap-config.cmake.in
                   ${BINARY_DIR}/wrap/wrap-config.cmake
                   @ONLY)
    INCLUDE(${BINARY_DIR}/wrap/wrap-config.cmake)
ENDIF ()


MESSAGE(STATUS "Looking for TypeART")
SET(HAVE_TYPEART_SUPPORT FALSE)
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    IF (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9)
        SET(HAVE_TYPEART_SUPPORT TRUE)
    ENDIF ()
ENDIF()
IF(HAVE_TYPEART_SUPPORT)
  MESSAGE(STATUS "Looking for TypeART - found")
ELSE()
  MESSAGE(STATUS "Looking for TypeART - not found")
ENDIF()
option(ENABLE_TYPEART "Use TypeART for memory allocation analysis" ${HAVE_TYPEART_SUPPORT})

IF (ENABLE_TYPEART)
    # Search for TypeART.
    #
    # By default, an existing and installed version of TypeART will be used for MUST.
    # However, if TypeART is not installed at this system, the TypeART sources will be
    # included as submodule.
    #
    IF (NOT DEFINED TYPEART_IS_SUBPROJECT)
      find_package(typeart CONFIG COMPONENTS Runtime)
    ENDIF ()

    IF (typeart_FOUND AND typeart_Runtime_FOUND)
        # As FIND_PACKAGE() needs to be called with the 'QUIET' flag to hide errors
        # about not found packages, we'll have to print the success message now.
        FIND_PACKAGE_MESSAGE(typeart_Runtime
                              "Found TypeART ${typeart_VERSION} runtime."
                              "Location: ${TYPEART_LIBRARY_DIR}")

        # No matter if MUST is dbg or rel build, we need to take
        # what TypeART system installation offers as wrapper script:
        if(TYPEART_MPICC_CMD_DEBUG AND NOT TYPEART_MPICC_CMD)
          set(TYPEART_MPICC_CMD ${TYPEART_MPICC_CMD_DEBUG})
        endif()
    ELSE ()
        IF (CMAKE_VERSION VERSION_LESS "3.20")
            set (ENABLE_TYPEART OFF)
            RETURN()
        ENDIF ()
        # If typeart is not installed at the system and its sources are not available
        # in the subdirectory, the user either needs to drop the typeart sources into
        # this subdirectory, or (if GTI sources are fetched from git) checkout the
        # git submodules.
        IF (NOT EXISTS ${SOURCE_DIR}/typeart/CMakeLists.txt)
            MESSAGE(FATAL_ERROR "Could NOT find typeart or its sources. Try updating "
                                "the git submodules or drop the sources of typeart "
                                "into ${SOURCE_DIR}/typeart")
        ENDIF ()

        # Add typeart as submodule and print a message with its path to CMake's
        # console. This message will be printed only once.
        set(TYPEART_MPI_INTERCEPT_LIB OFF)
        set(TYPEART_CONFIG_DIR_IS_SHARE ON)

        ADD_SUBDIRECTORY(${SOURCE_DIR}/typeart)

        mark_as_advanced(
            TYPEART_ABSEIL
            TYPEART_ASAN
            TYPEART_CODE_COVERAGE
            TYPEART_LLVMCOV_COMMAND
            TYPEART_LLVM_CODE_COVERAGE
            TYPEART_LOG_LEVEL
            TYPEART_LOG_LEVEL_RT
            TYPEART_MPI_INTERCEPT_LIB
            TYPEART_MPI_LOGGER
            TYPEART_SAFEPTR
            TYPEART_SOFTCOUNTERS
            TYPEART_TEST_CONFIG
            TYPEART_TSAN
            TYPEART_UBSAN
        )

        # This script only exists with add_subdir but handles "build-dir" paths of
        # TypeART correctly
        SET(TYPEART_MPICC_CMD "${BINARY_DIR}/typeart/scripts/typeart-mpicc-test")

        # Used for same reason as GTI project
        option(TYPEART_IS_SUBPROJECT "Whether TypeART \"external\" directory sources will be used" ON)
        MARK_AS_ADVANCED(TYPEART_IS_SUBPROJECT)
    ENDIF ()
ENDIF ()
