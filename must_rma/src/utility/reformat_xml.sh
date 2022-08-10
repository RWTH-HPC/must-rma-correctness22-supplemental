#!/bin/bash

# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause


for i in ../specifications/*.xml
do
    if cat $i | grep -e '[^ ]' | sed -e 's/\t/ /g' -e 's/[ ][ ]*/ /g' | sed -e 's/> *</>\n</g' -e 's/[ ]*$//' | sed -e '{:q;N;s/\([^>]\)\n[ ]*/\1 /g;s/\([^>]\)$/\1/g;t q}' | xmlindent -i 4 -l 160 > $i.tmp ; then
        if diff $i $i.tmp > /dev/null; then
            rm $i.tmp
        else
            mv $i.tmp $i
            echo $i reformated
        fi
    fi
done
