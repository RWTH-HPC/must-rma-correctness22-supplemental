/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TSanSignalRecorder.h
 * Signal recorder module: Stores TSan SyncClock addresses (= plain memory addresses)
 * together with the GTI vector clock value (i.e., the value from the VectorClock module)
 *
 *  @date 10.11.2021
 *  @author Simon Schwitanski
 */


#include "ModuleBase.h"
#include "GtiEnums.h"
#include "I_Module.h"
#include "I_TSanSyncClockRecorder.h"
#include "I_VectorClock.h"
#include <map>

#ifndef TSANSYNCCLOCKRECORDER_H
#define TSANSYNCCLOCKRECORDER_H


using namespace gti;

namespace must
{

class TSanSyncClockRecorder : public gti::ModuleBase<TSanSyncClockRecorder, I_TSanSyncClockRecorder>
{
  public:
    /**
      * Constructor.
      * @param instanceName name of this module instance.
      */
    TSanSyncClockRecorder (const char* instanceName);

    /**
      * @see I_TSanSyncClockRecorder:getSyncClock
      */
    void* getSyncClock(int clock) const;

    /**
      * @see I_TSanSyncClockRecorder:storeSyncClock
      */
    GTI_ANALYSIS_RETURN storeSyncClock(void* ann);

  private:
    std::unordered_map<unsigned long long, void*> syncClocks; // map local clock values to sync clock addresses
    I_VectorClock* myVCMod;
     
}; /*class TSanSyncClockRecorder*/

}

#endif /* TSANSYNCCLOCKRECORDER_H */
