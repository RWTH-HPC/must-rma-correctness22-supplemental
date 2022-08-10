/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file InitParallelIdMpi.cpp
 *       @see must::InitParallelIdMpi.
 *
 *  @date 16.04.2014
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"

#include "InitParallelIdMpi.h"

using namespace must;

mGET_INSTANCE_FUNCTION(InitParallelIdMpi)
mFREE_INSTANCE_FUNCTION(InitParallelIdMpi)
mPNMPI_REGISTRATIONPOINT_FUNCTION(InitParallelIdMpi)

//=============================
// Constructor
//=============================
InitParallelIdMpi::InitParallelIdMpi (const char* instanceName)
    : gti::ModuleBase<InitParallelIdMpi, I_InitParallelId> (instanceName),
      myInitedId (false),
      myPId (0)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();
    //No sub modules needed ...
}

//=============================
// init
//=============================
GTI_ANALYSIS_RETURN InitParallelIdMpi::init (MustParallelId *pStorage)
{
    if (!pStorage)
        return GTI_ANALYSIS_FAILURE;

    //Init if necessary
    if(!myInitedId)
    {
        /**
         * In our MPI specific case the pId is exactly equal to the layer id from GTI, so we use that one.
         */
        myPId = this->buildLayer ();
    }

    //Store it
    *pStorage = myPId;

    return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
