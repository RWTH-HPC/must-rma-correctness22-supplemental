/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DCollectiveInitNotify.cpp
 *       @see MUST::DCollectiveInitNotify.
 *
 *  @date 03.05.2012
 *  @author Tobias Hilbrich, Mathias Korepkat, Fabian Haensel, Joachim Protze
 */

#include "GtiMacros.h"
#include "DCollectiveInitNotify.h"
#include "DistributedDeadlockApi.h"

using namespace must;

mGET_INSTANCE_FUNCTION(DCollectiveInitNotify)
mFREE_INSTANCE_FUNCTION(DCollectiveInitNotify)
mPNMPI_REGISTRATIONPOINT_FUNCTION(DCollectiveInitNotify)

//=============================
// Constructor
//=============================
DCollectiveInitNotify::DCollectiveInitNotify (const char* instanceName)
    : gti::ModuleBase<DCollectiveInitNotify, I_DCollectiveInitNotify> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
    //None
}

//=============================
// Destructor
//=============================
DCollectiveInitNotify::~DCollectiveInitNotify ()
{
    //TODO
}

//=============================
// notifyInit
//=============================
GTI_ANALYSIS_RETURN DCollectiveInitNotify::notifyInit (void)
{
    dCollMatchAncestorHasIntraP f;
    getWrapperFunction ("dCollMatchAncestorHasIntra", (GTI_Fct_t*)&f);

    if (f)
        (*f) (0);

	return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
