# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# - Find Dot
# This module looks for dot of the graphviz toolset.
# It will set the following variables:
#  DOT = the dot binary
#
# @author Mathias Korepkat
#

INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM (DOT NAMES dot PATH /usr/bin /bin /local/bin /usr/local/bin)
MARK_AS_ADVANCED(FORCE DOT)

find_package_handle_standard_args(DOT  DEFAULT_MSG DOT)
