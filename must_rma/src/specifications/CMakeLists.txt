# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for the specifications directory.
#
# @author Tobias Hilbrich
# @date 03.01.2011

if (GTI_OMPT_FOUND)
    set(HAVE_OMPT_H_PREFIX "")
    set(HAVE_OMPT_H_POSTFIX "")
else (GTI_OMPT_FOUND)
    set(HAVE_OMPT_H_PREFIX "<!--")
    set(HAVE_OMPT_H_POSTFIX "-->")
endif(GTI_OMPT_FOUND)


SET (ArgId_TYPE "int" CACHE INTERNAL "Type of the argument ids.")


# Configure and install all specifications.
#
# NOTE: Specification files will be searched dynamically to support excluding
#       specific specifications by simply removing its file (i.e. to not ship an
#       experimental module's specification on deploy).
#
# NOTE: The xmls that are configured should have a ".in" at the end, but this
#       will disable editor support for them, so we didn't add this extension.
FILE(GLOB SPECIFICATIONS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
          "*.xml"
          "optional/*.xml")
FOREACH (SPEC ${SPECIFICATIONS})
    CONFIGURE_FILE(${SPEC} ${SPEC} @ONLY)
    MUST_INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SPEC}
                 DESTINATION specifications)
ENDFOREACH ()


CONFIGURE_FILE (p_elp_api.h ${CMAKE_CURRENT_BINARY_DIR}/p_elp_api.h @ONLY)
MUST_INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/p_elp_api.h DESTINATION include)
