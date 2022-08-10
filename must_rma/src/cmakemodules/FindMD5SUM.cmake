# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause
# 
# - Find md5sum
# This module looks for the md5sum command. 
# It will set the following variables:
#  MD5SUM = the md5sum command if found
#
# @author Tobias Hilbrich
#

INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM (MD5SUM NAMES md5sum PATH /usr/bin /bin /local/bin /usr/local/bin)
MARK_AS_ADVANCED(FORCE MD5SUM)

find_package_handle_standard_args(MD5SUM  DEFAULT_MSG MD5SUM)