# Part of the MUST Project, under BSD-3-Clause License
# See https://hpc.rwth-aachen.de/must/LICENSE for license information.
# SPDX-License-Identifier: BSD-3-Clause

# @file CMakeLists.cmake
#       CMake file for LeakChecks directory.
#
# @author Tobias Hilbrich, Mathias Korepkat
# @date 19.05.2011
#

##################
#Recurse into sub directories
##Not needed

###################
#Add test cases

##No Error
addProcessLocalTest ("LeaksNoError" "LeaksNoError.c" 2 
    ""
    "Error;ERROR;Warning;WARNING"
    )
    
## Error: leaked comm
addProcessLocalTest ("LeakCommError" "LeakCommError.c" 2 
    "There are 1 communicators that are not freed when MPI_Finalize"
    ""
    )
    
## Error: leaked group
addProcessLocalTest ("LeakGroupError" "LeakGroupError.c" 2 
    "There are 1 groups that are not freed when MPI_Finalize was issued"
    ""
    )
    
## Error: leaked errorhandler
addProcessLocalTest ("LeakErrError" "LeakErrError.c" 2 
    "There are 1 error handlers that are not freed when MPI_Finalize was"
    ""
    )
    
## Error: leaked keyvalue
addProcessLocalTest ("LeakKeyvalError" "LeakKeyvalError.c" 2 
    "There are 1 keys that are not freed when MPI_Finalize was issued"
    ""
    )
    
## Error: leaked operation
addProcessLocalTest ("LeakOpError" "LeakOpError.c" 2 
    "There are 1 operations that are not freed when MPI_Finalize"
    ""
    )

## Error: leaked request
addProcessLocalTest ("LeakRequestError" "LeakRequestError.c" 2 
    "There are 1 requests that are not freed when MPI_Finalize "
    ""
    )
    
## Error: leaked datatype
addProcessLocalTest ("LeakTypeError" "LeakTypeError.c" 2 
    "There are 1 datatypes that are not freed when MPI_Finalize"
    ""
    )