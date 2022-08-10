/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TSanSyncClockRecorder.cpp
 *       @see must::TSanSyncClockRecorder.
 *
 *  @date 12.11.2021
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"
#include "TSanSyncClockRecorder.h"
#include "MustEnums.h"
#include "MustDefines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#include <iostream>

using namespace must;

mGET_INSTANCE_FUNCTION(TSanSyncClockRecorder)
mFREE_INSTANCE_FUNCTION(TSanSyncClockRecorder)
mPNMPI_REGISTRATIONPOINT_FUNCTION(TSanSyncClockRecorder)


//=============================
// Constructor
//=============================
TSanSyncClockRecorder::TSanSyncClockRecorder (const char* instanceName)
    : ModuleBase<TSanSyncClockRecorder, I_TSanSyncClockRecorder> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUBMODULES 1
    if (subModInstances.size() < NUM_SUBMODULES)
    {
            std::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
            assert (0);
    }
    if (subModInstances.size() > NUM_SUBMODULES)
    {
            for (std::vector<I_Module*>::size_type i = NUM_SUBMODULES; i < subModInstances.size(); i++)
                destroySubModuleInstance (subModInstances[i]);
    }

    myVCMod = (I_VectorClock*) subModInstances[0];
}


//=============================
// getSyncClock
//=============================
void* TSanSyncClockRecorder::getSyncClock(int clock) const {
#ifdef MUST_DEBUG
    std::cout << "[SyncClockRecorder] Look up for clock value " << clock 
              << std::endl;
#endif
    auto it = syncClocks.find(clock);
    assert(it != syncClocks.end());

    return it->second;
}


//=============================
// storeSyncClock
//=============================
GTI_ANALYSIS_RETURN TSanSyncClockRecorder::storeSyncClock(void* ann)
{
    // Note: annotateSignal and this storeSyncClock call are invoked
    // *before* the VC tick, so we have to add up one here.
    syncClocks[myVCMod->getLocalClockValue()+1] = ann;
#ifdef MUST_DEBUG
    std::cout << "[SyncClockRecorder] Store ann=" << ann 
              << " with clock value " << myVCMod->getLocalClockValue()+1 
              << std::endl;
#endif

    return GTI_ANALYSIS_SUCCESS;
}
