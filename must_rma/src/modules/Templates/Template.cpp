/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file Template.cpp
 *       @see MUST::Template.
 *
 *  @date 21.01.2011
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"

#include "Template.h"

using namespace must;

mGET_INSTANCE_FUNCTION(Template);
mFREE_INSTANCE_FUNCTION(Template);
mPNMPI_REGISTRATIONPOINT_FUNCTION(Template);

//=============================
// Constructor
//=============================
Template::Template (const char* instanceName)
    : gti::ModuleBase<Template, I_Template> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
    //TODO
}

//=============================
// Destructor
//=============================
Template::~Template ()
{
    //TODO
}

//=============================
//
//=============================
GTI_ANALYSIS_RETURN Template::analysisFunction (void)
{
	//TODO

	return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
