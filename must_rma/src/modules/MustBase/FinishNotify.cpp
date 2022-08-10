/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file FinishNotify.cpp
 *       @see MUST::FinishNotify.
 *
 *  @date 05.08.2013
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "FinishNotify.h"
#include "MustEnums.h"
//#include "PrefixedOstream.hpp"

#include <sstream>

using namespace must;

mGET_INSTANCE_FUNCTION(FinishNotify)
mFREE_INSTANCE_FUNCTION(FinishNotify)
mPNMPI_REGISTRATIONPOINT_FUNCTION(FinishNotify)

//=============================
// Constructor
//=============================
FinishNotify::FinishNotify (const char* instanceName)
    : gti::ModuleBase<FinishNotify, I_FinishNotify> (instanceName),
      myListeners ()
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUBS 0
//    if (subModInstances.size() < NUM_SUBS)
//    {
//    		must::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
//    		assert (0);
//    }
    if (subModInstances.size() > NUM_SUBS)
    {
        for (std::vector<I_Module*>::size_type i = NUM_SUBS; i < subModInstances.size(); i++)
            destroySubModuleInstance (subModInstances[i]);
    }
}

//=============================
// Destructor
//=============================
FinishNotify::~FinishNotify ()
{
    //Nothing to do
}

//=============================
// finish
//=============================
GTI_ANALYSIS_RETURN FinishNotify::finish (void)
{
    std::list<I_FinishListener*>::iterator iter;
    for (iter = myListeners.begin(); iter != myListeners.end(); iter++)
    {
        (*iter)->finish();
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addListener
//=============================
void FinishNotify::addListener (I_FinishListener *listener)
{
    myListeners.push_back (listener);
}

/*EOF*/
