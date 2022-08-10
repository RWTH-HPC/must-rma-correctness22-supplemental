/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file ParallelIdImpl.cpp
 *       Implementation for the parallel id analysis interface.
 *
 *  @date 07.01.2010
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"

#include "ParallelIdImpl.h"

using namespace must;

mGET_INSTANCE_FUNCTION(ParallelIdImpl)
mFREE_INSTANCE_FUNCTION(ParallelIdImpl)
mPNMPI_REGISTRATIONPOINT_FUNCTION(ParallelIdImpl)

//=============================
// Constructor
//=============================
ParallelIdImpl::ParallelIdImpl (const char* instanceName)
    : gti::ModuleBase<ParallelIdImpl, I_ParallelIdAnalysis, false> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();
    //No sub modules needed ...
}

//=============================
// getInfoForId
//=============================
ParallelInfo ParallelIdImpl::getInfoForId (MustParallelId id)
{
	/*
	 * Will be enhanced along the road.
	 */

	ParallelInfo ret;
        ret.rank = ( id & 0xFFFFFFFF );
        ret.threadid = ( id >> 32 );
	return ret;
}


//=============================
// toString
//=============================
std::string ParallelIdImpl::toString (MustParallelId id)
{
	ParallelInfo info = getInfoForId (id);
	std::string ret = "";

        char temp[128];
	sprintf (temp, "%d", info.rank);
	ret += (std::string)("rank ") + temp + ", ";
        sprintf (temp, "%d", info.threadid);
        ret += (std::string)("threadid ") + temp;

	return ret;
}

/*EOF*/
