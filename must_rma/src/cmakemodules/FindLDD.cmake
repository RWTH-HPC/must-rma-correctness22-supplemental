# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause
# 
# - Find LDD
# This module looks for LDD. 
# It will set the following variables:
#  LDD = the ldd binary
#
# @author Joachim Protze
#

INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM (LDD NAMES ldd PATH /usr/bin /bin /local/bin /usr/local/bin)
MARK_AS_ADVANCED(FORCE LDD)

find_package_handle_standard_args(LDD  DEFAULT_MSG LDD)