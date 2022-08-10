/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file InitParallelIdHybrid.cpp
 *       @see must::InitParallelIdHybrid.
 *
 *  @date 16.04.2014
 *  @author Felix Muenchhalfen
 */

#include "GtiMacros.h"

#include "InitParallelIdHybrid.h"

using namespace must;

mGET_INSTANCE_FUNCTION(InitParallelIdHybrid)
mFREE_INSTANCE_FUNCTION(InitParallelIdHybrid)
mPNMPI_REGISTRATIONPOINT_FUNCTION(InitParallelIdHybrid)

//=============================
// Constructor
//=============================
InitParallelIdHybrid::InitParallelIdHybrid (const char* instanceName)
    : gti::ModuleBase<InitParallelIdHybrid, I_InitParallelId> (instanceName),
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
GTI_ANALYSIS_RETURN InitParallelIdHybrid::init (MustParallelId *pStorage)
{
    if (!pStorage)
        return GTI_ANALYSIS_FAILURE;

    //Init if necessary
    if(!myInitedId)
    {
        /**
         * In our MPI specific case the pId is exactly equal to the layer id from GTI, so we use that one.
         */
//        myPId = buildLayer(true);
        getNodeInLayerId(&myPId);
    }

    //Store it
    *pStorage = myPId;

    return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
