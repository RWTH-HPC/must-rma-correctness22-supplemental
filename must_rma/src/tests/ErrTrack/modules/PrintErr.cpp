/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file PrintErr.cpp
 *       @see MUST::PrintErr.
 *
 *  @date 12.05.2011
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "PrintErr.h"

#include <assert.h>
#include <sstream>

using namespace must;

mGET_INSTANCE_FUNCTION(PrintErr);
mFREE_INSTANCE_FUNCTION(PrintErr);
mPNMPI_REGISTRATIONPOINT_FUNCTION(PrintErr);

//=============================
// Constructor
//=============================
PrintErr::PrintErr (const char* instanceName)
    : gti::ModuleBase<PrintErr, I_PrintErr> (instanceName),
      myLogger (NULL),
      myErrTracker (NULL),
      myLocations (NULL)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
    if (subModInstances.size () < 3)
    {
    		std::cerr << "Error: " << __LINE__ << "@" << __FILE__ << " has not enough sub modules, aborting!" << std::endl;
    		assert (0);
    }

    myLocations = (I_LocationAnalysis*) subModInstances[0];
    myLogger = (I_CreateMessage*) subModInstances[1];
    myErrTracker = (I_ErrTrack*) subModInstances[2];
}

//=============================
// Destructor
//=============================
PrintErr::~PrintErr ()
{
	if (myLocations)
		destroySubModuleInstance ((I_Module*) myLocations);
	myLocations = NULL;

    if (myLogger)
    		destroySubModuleInstance ((I_Module*) myLogger);
    myLogger = NULL;

    if (myErrTracker)
    		destroySubModuleInstance ((I_Module*) myErrTracker);
    myErrTracker = NULL;
}

//=============================
// print
//=============================
GTI_ANALYSIS_RETURN PrintErr::print (
		MustParallelId pId,
		MustLocationId lId,
		MustErrType err)
{
    static int index=0;
	std::stringstream stream;
	stream << "Information on error handler: ";
	std::list<std::pair<MustParallelId,MustLocationId> > refs;
	I_Err* info = myErrTracker->getErr(pId,err);

	if (info)
	{
	    info->printInfo(stream, &refs);
	}
	else
	{
	    stream << "Unknown Errorhandler.";
	}

	myLogger->createMessage(index++, pId, lId, MustInformationMessage, stream.str(), refs);

	return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
