/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DWaitStateCollMgr.cpp
 *       @see DWaitStateCollMgr.
 *
 *  @date 05.03.2013
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "MustEnums.h"
#include "PrefixedOstream.hpp"

#include "DWaitStateCollMgr.h"

#include <assert.h>

using namespace must;

mGET_INSTANCE_FUNCTION(DWaitStateCollMgr)
mFREE_INSTANCE_FUNCTION(DWaitStateCollMgr)
mPNMPI_REGISTRATIONPOINT_FUNCTION(DWaitStateCollMgr)

//=============================
// Constructor
//=============================
DWaitStateCollMgr::DWaitStateCollMgr (const char* instanceName)
    : gti::ModuleBase<DWaitStateCollMgr, I_DWaitStateCollMgr> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUBS 2
    if (subModInstances.size() < NUM_SUBS)
    {
            must::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
            assert (0);
    }
    if (subModInstances.size() > NUM_SUBS)
    {
            for (std::vector<I_Module*>::size_type i = NUM_SUBS; i < subModInstances.size(); i++)
                destroySubModuleInstance (subModInstances[i]);
    }

    myPIdMod = (I_ParallelIdAnalysis*) subModInstances[0];
    myCommTrack = (I_CommTrack*) subModInstances[1];

    //Initialize module data
    ModuleBase<DWaitStateCollMgr, I_DWaitStateCollMgr>::getBroadcastFunction("generateCollectiveActiveAcknowledge", (GTI_Fct_t*)&myFAcknowledge);
    assert (myFAcknowledge); //Must be there, otherwise we have a mapping error for this tool configuration
}

//=============================
// Destructor
//=============================
DWaitStateCollMgr::~DWaitStateCollMgr ()
{
    /*Free other data*/
    if (myPIdMod)
        destroySubModuleInstance ((I_Module*) myPIdMod);
    myPIdMod = NULL;

    if (myCommTrack)
        destroySubModuleInstance ((I_Module*) myCommTrack);
    myCommTrack = NULL;

    //Free module data
    /*Nothing to do*/
}

//=============================
// request
//=============================
GTI_ANALYSIS_RETURN DWaitStateCollMgr::request (
        int isIntercomm,
        unsigned long long contextId,
        int collCommType,
        int localGroupSize,
        int remoteGroupSize,
        int numTasks,
        I_ChannelId* cId)
{
    //Check whether this really is a complete request, as it must due to DWaitStateCollReduction's design
    assert (cId == NULL);

    if (myFAcknowledge)
    {
        (*myFAcknowledge) (
                isIntercomm,
                contextId,
                localGroupSize,
                remoteGroupSize
            );
    }

    return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
