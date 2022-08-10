/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file FinalizeMust.cpp
 *       @see MUST::FinalizeMust.
 *
 *  @date 05.03.2011
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "GtiApi.h"
#include "MustEnums.h"
#include "BaseApi.h"
#include "PrefixedOstream.hpp"

#include "FinalizeMust.h"

using namespace must;

mGET_INSTANCE_FUNCTION(FinalizeMust)
mFREE_INSTANCE_FUNCTION(FinalizeMust)
mPNMPI_REGISTRATIONPOINT_FUNCTION(FinalizeMust)

//=============================
// Constructor
//=============================
FinalizeMust::FinalizeMust (const char* instanceName)
    : gti::ModuleBase<FinalizeMust, I_FinalizeMust> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

#define NUM_SUBS 0
    //handle sub modules
#if NUM_SUBS>0
    if (subModInstances.size() < NUM_SUBS)
    {
    		must::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
    		assert (0);
    }
#endif
    if (subModInstances.size() > NUM_SUBS)
    {
    		for (unsigned int i = NUM_SUBS; i < subModInstances.size(); i++)
    			destroySubModuleInstance (subModInstances[i]);
    }
}

//=============================
// Destructor
//=============================
FinalizeMust::~FinalizeMust ()
{
	/*Nothing to do*/
}

//=============================
// notify
//=============================
GTI_ANALYSIS_RETURN FinalizeMust::notify ()
{
    //Create the finalize event
    finalizeMUSTP fP;
	if (getWrapperFunction ("finalizeMUST", (GTI_Fct_t*)&fP) == GTI_SUCCESS)
	{
		(*fP) ();
	}

	//Make sure we forward the finalize event immediately
	gtiNotifyFlushP f;
	if (getBroadcastFunction("gtiNotifyFlush", (GTI_Fct_t*) &f) == GTI_SUCCESS)
	    (*f) ();

	return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
