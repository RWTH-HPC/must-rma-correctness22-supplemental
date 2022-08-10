# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# Define the default MPI interface to be intercepted.
#
# By default, mustrun will intercept the MPI C interface, which should be fine
# for most recent MPI implementations (even for Fortran). However, if one needs
# to switch the interface to be intercepted many times, one can change the
# default interface by this switch, too.
SET(MUST_DEFAULT_MPI_INTERFACE "c"
    CACHE STRING "The default MPI interface to be intercepted.")

set(MUST_TIMEOUT_SCRIPT "${CMAKE_INSTALL_PREFIX}/bin/timeout" CACHE INTERNAL "")
# Use the more robust timeout executable provided by the system if possible. Use
# the script provided by mustrun as fallback if timeout can't be found in the
# system path.
find_program(TIMEOUT_EXECUTABLE
    # gtimeout is for macOS users that installed coreutils via brew
    timeout gtimeout
    DOC "the timeout executable used by mustrun"
    )
if(TIMEOUT_EXECUTABLE)
    # We assume the cli of GNU coreutils timeout. This might not work for other
    # implementations of timeout.
    set(TIMEOUT_COMMAND
	"${TIMEOUT_EXECUTABLE} --kill-after 10 --preserve-status")
else()
    set(TIMEOUT_COMMAND "${MUST_TIMEOUT_SCRIPT} -t")
endif()

# Configure mustrun.
#
# As the mustrun script needs paths of GTI binaries and one can't simply get
# these via the target properties (see CMake CMP0026), generator expressions
# will be used to get the location of the binaries. Therefore, mustrun will be
# configured first to replace all variables inside the script. Then, the
# generator expressions will be evaluated.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mustrun2.sh"
               "${CMAKE_CURRENT_BINARY_DIR}/mustrun.configured" @ONLY)
FILE(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/mustrun"
              INPUT  "${CMAKE_CURRENT_BINARY_DIR}/mustrun.configured")

add_library(PrintMemUsage SHARED
  printMemoryusage.c
)

add_library(XPrintMemUsage SHARED
  printMemoryusage.c
)

make_mpi_target(PrintMemUsage)
make_mpi_target(XPrintMemUsage)

if(APPLE)
    get_target_property(TEMP_FLAG XPrintMemUsage LINK_FLAGS)
    string(TOUPPER "${TEMP_FLAG}" TEMP_CAPITAL_FLAG)
    if(${TEMP_CAPITAL_FLAG} STREQUAL "TEMP_FLAG-NOTFOUND")
        set(TEMP_FLAG "")
    endif()
    set(TEMP_FLAG "${TEMP_FLAG} -undefined dynamic_lookup")
    set_target_properties(XPrintMemUsage PROPERTIES LINK_FLAGS ${TEMP_FLAG})
endif()

target_compile_definitions( XPrintMemUsage PRIVATE -DPMPIPREFIX=XMPI)

INSTALL(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/mustrun"
        "timeout"
        "cleanup_shm"
    DESTINATION bin 
    PERMISSIONS 
        OWNER_READ OWNER_WRITE OWNER_EXECUTE  
        GROUP_READ GROUP_EXECUTE  
        WORLD_READ WORLD_EXECUTE
    )

INSTALL(TARGETS
	PrintMemUsage XPrintMemUsage
    DESTINATION modules
    PERMISSIONS 
        OWNER_READ OWNER_WRITE 
        GROUP_READ 
        WORLD_READ 
    )

INSTALL(
    FILES "error_generator.c" 
    DESTINATION src
    )
