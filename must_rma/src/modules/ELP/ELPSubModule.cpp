/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file ELPSubModule.cpp
 *       @see MUST::ELPSubModule.
 *
 *  @date 22.07.2014
 *  @author Felix Muenchhalfen
 */

#include "GtiMacros.h"
#include "ELPSubModule.h"
#include "MustEnums.h"
#include "PrefixedOstream.hpp"

using namespace must;

mGET_INSTANCE_FUNCTION(ELPSubModule)
mFREE_INSTANCE_FUNCTION(ELPSubModule)
mPNMPI_REGISTRATIONPOINT_FUNCTION(ELPSubModule)

ELPSubModule::ELPSubModule(const char* instanceName) : gti::ModuleBase<ELPSubModule, I_ELPSubModule> (instanceName) 
{
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_MODULES 0
    if (subModInstances.size() < NUM_MODULES)
    {
    		must::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
    		assert (0);
    }
    if (subModInstances.size() > NUM_MODULES)
    {
    		for (int i = NUM_MODULES; i < subModInstances.size(); i++)
    			destroySubModuleInstance (subModInstances[i]);
    }

    //myLogger = (I_CreateMessage*)subModInstances[0];

    //Initialize module data
    //Nothing to do
}

ELPSubModule::~ELPSubModule() {
    /*if( myLogger != NULL )
        destroySubModuleInstance( myLogger );
    myLogger = NULL;*/
}

