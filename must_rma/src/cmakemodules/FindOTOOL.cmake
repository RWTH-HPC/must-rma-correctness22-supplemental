# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause
# 
# - Find OTOOL
# This module looks for OTOOL. 
# It will set the following variables:
#  OTOOL = the otool binary
#
# @author Joachim Protze
#

INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM (OTOOL NAMES otool PATH /usr/bin /bin /local/bin /usr/local/bin)
MARK_AS_ADVANCED(FORCE OTOOL)

find_package_handle_standard_args(OTOOL  DEFAULT_MSG OTOOL)