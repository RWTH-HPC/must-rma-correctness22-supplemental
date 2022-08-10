# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# - Find AWK
# This module looks for AWK.
# It will set the following variables:
#  AWK = the awk binary
#
# @author Tobias Hilbrich
#

INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM (AWK NAMES awk gawk PATH /usr/bin /bin /local/bin /usr/local/bin)
MARK_AS_ADVANCED(FORCE AWK)

find_package_handle_standard_args(AWK  DEFAULT_MSG AWK)
