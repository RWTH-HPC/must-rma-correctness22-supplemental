#!/bin/bash

# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

i=0
for j in $(grep ' size' $1| sed -e 's|.*size="\([^"]*\)".*|\1|')
do 
	i=$[$i+$j]
done
echo $i
