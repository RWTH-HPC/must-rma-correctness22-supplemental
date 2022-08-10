/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_TSanSignalRecorder.h
 * Signal recorder module: Stores TSan SyncClock addresses (= plain memory addresses)
 * together with the GTI vector clock value (i.e., the value from the VectorClock module)
 *
 *  @date 10.11.2021
 *  @author Simon Schwitanski
 */


#include "GtiEnums.h"

#include "MustEnums.h"
#include "BaseIds.h"
#include "MustTypes.h"
#include <vector>

#ifndef I_TSANSYNCCLOCKRECORDER_H
#define I_TSANSYNCCLOCKRECORDER_H

/**
 *
 *
 * Dependencies (order as listed):
 * - I_VectorClock
 *
 */
class I_TSanSyncClockRecorder : public gti::I_Module
{
  public:
    /**
     * Returns the TSan sync clock address of the given MPI clock value.
     *
     * @param clock clock value
     * @return TSan sync clock address
     */
    virtual void* getSyncClock(int clock) const = 0;

    /**
     * Stores the TSan sync clock address together with the given MPI clock value.
     * 
     * @param pId 
     * @param lId 
     * @param ann TSan sync clock address
     * @return gti::GTI_ANALYSIS_RETURN 
     */
    virtual gti::GTI_ANALYSIS_RETURN storeSyncClock(void* ann) = 0;

}; /*class I_TSanSyncClockRecorder*/

#endif /* I_TSANSYNCCLOCKRECORDER_H */
