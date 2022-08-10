/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OMPTStartup.cpp
 * 
 *
 * @author Felix Münchhalfen
 * @date 09.07.2014
 *
 */


#include "GtiMacros.h"
#include "OMPTStartup.h"
#include "PrefixedOstream.hpp"

using namespace gti;

mGET_INSTANCE_FUNCTION(OMPTIntegration);
mFREE_INSTANCE_FUNCTION(OMPTIntegration);
mPNMPI_REGISTRATIONPOINT_FUNCTION(OMPTIntegration);

#ifdef GTI_OMPT_FOUND
ompt_function_lookup_t OMPTIntegration::myompt_fn_lookup = NULL;
ompt_set_callback_t OMPTIntegration::myompt_fn_setcallback = NULL;
#endif
/* -----------------------------------------------------------------------------
 * Integration of OMPT
 */

OMPTIntegration::OMPTIntegration (const char* instanceName)
    : gti::ModuleBase<OMPTIntegration, I_OMPTIntegration> (instanceName)
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

OMPTIntegration::~OMPTIntegration()
{
    
}

#ifdef GTI_OMPT_FOUND
void OMPTIntegration::OMPTInitialization( ompt_function_lookup_t fn_lookup )
{
}

GTI_ANALYSIS_RETURN OMPTIntegration::getLookUpFunction( ompt_function_lookup_t *func )
{
    *func = myompt_fn_lookup;
    return GTI_ANALYSIS_SUCCESS;
}

GTI_ANALYSIS_RETURN OMPTIntegration::setCallback( ompt_event_t event, ompt_callback_t callback)
{
    myompt_fn_setcallback(event, callback);
    return GTI_ANALYSIS_SUCCESS;
}

void OMPTIntegration::ompt_init_callback( ompt_function_lookup_t ompt_fn_lookup,
                            const char *runtime_version,
                            unsigned int ompt_version
                          )
{
    printf("OMPT WRAPPER FUNCTION CALLED!\n");
    OMPTIntegration::myompt_fn_lookup = ompt_fn_lookup;
    OMPTIntegration::myompt_fn_setcallback = (ompt_set_callback_t) ompt_fn_lookup("ompt_set_callback");
    printf("OMPT init success\n");
}

/*extern "C" void ompt_init_callback( ompt_function_lookup_t ompt_fn_lookup,
                            const char *runtime_version,
                            unsigned int ompt_version
                          )
{
    printf("OMPT WRAPPER FUNCTION CALLED!\n");
    OMPTIntegration::myompt_fn_lookup = ompt_fn_lookup;
    OMPTIntegration::myompt_fn_setcallback = (ompt_set_callback_t) ompt_fn_lookup("ompt_set_callback");
    printf("OMPT init success\n");
}

/*extern "C" ompt_initialize_t Xompt_tool();
extern "C" ompt_initialize_t ompt_tool()
{
    printf("OMPT TOOL FUNCTION CALLED!\n");
    return ompt_init_callback;
}*/
#endif

/* End of Integration of OMPT
 */

/*EOF*/
