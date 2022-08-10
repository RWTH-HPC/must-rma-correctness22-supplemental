/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file I_VectorClockWrapper.h
 *       Vector clock analysis module.
 *
 *  @date 29.06.2021
 *  @author Felix Tomski
 */

#include "BaseIds.h"
#include "GtiEnums.h"
#include "I_Module.h"

#ifndef I_VECTORCLOCKWRAPPER_H
#define I_VECTORCLOCKWRAPPER_H

/**
 * Wrapper for the GTI VectorClock module.
 *
 * Dependencies (order as listed):
 * - X
 *
 */
class I_VectorClockWrapper : public gti::I_Module {
  public:
    /**
     * Increment own vector clock entry by 1.
     *
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN init() = 0;

    virtual gti::GTI_ANALYSIS_RETURN tick() = 0;

    virtual gti::GTI_ANALYSIS_RETURN signal(MustParallelId pId, int isSync, int appRank, int tag,
                                            MustCommType comm) = 0;

    virtual gti::GTI_ANALYSIS_RETURN wait(MustParallelId pId, int appRank, int tag,
                                          MustCommType comm) = 0;

    virtual gti::GTI_ANALYSIS_RETURN waitForResponse(MustParallelId pId, int appRank, int tag,
                                                     MustCommType comm) = 0;

    virtual gti::GTI_ANALYSIS_RETURN allToOne(MustParallelId pId, MustCommType comm, int root) = 0;

    virtual gti::GTI_ANALYSIS_RETURN oneToAll(MustParallelId pId, MustCommType comm, int root) = 0;

    virtual gti::GTI_ANALYSIS_RETURN allToAll(MustParallelId pId, MustCommType comm) = 0;

    /**
     * Can't save source and tag here because of wildcards.
     *
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN bufferWait(MustParallelId pId, MustCommType comm,
                                                MustRequestType request) = 0;

    virtual gti::GTI_ANALYSIS_RETURN bufferA2aClock(MustParallelId pId, MustCommType comm,
                                                    MustRequestType request) = 0;

    virtual gti::GTI_ANALYSIS_RETURN bufferA2oClock(MustParallelId pId, int root, MustCommType comm,
                                                    MustRequestType request) = 0;

    virtual gti::GTI_ANALYSIS_RETURN bufferO2aClock(MustParallelId pId, int root, MustCommType comm,
                                                    MustRequestType request) = 0;

    virtual gti::GTI_ANALYSIS_RETURN handleRequest(MustRequestType request, int source,
                                                   int tag) = 0;
    virtual gti::GTI_ANALYSIS_RETURN handleRequestArray(MustRequestType* requests, int* sources,
                                                        int* tags, size_t count) = 0;
    virtual gti::GTI_ANALYSIS_RETURN handleAnyRequest(MustRequestType* requests, int index,
                                                      int source, int tag) = 0;

    virtual gti::GTI_ANALYSIS_RETURN winAllToAll(MustParallelId pId, MustWinType winHandle) = 0;

    virtual gti::GTI_ANALYSIS_RETURN lock(MustParallelId pId, MustWinType winHandle,
                                          int appRank, int lock_type) = 0;
    virtual gti::GTI_ANALYSIS_RETURN unlock(MustParallelId pId, MustWinType winHandle,
                                            int appRank) = 0;

    virtual gti::GTI_ANALYSIS_RETURN addPersistentSendInfo(MustParallelId pId, int appRank, int tag,
                                                           unsigned long requestHandle,
                                                           MustCommType comm) = 0;


    /* general active target (MPI RMA) */
    virtual gti::GTI_ANALYSIS_RETURN winStart(MustParallelId pId, MustLocationId lId, MustGroupType groupHandle, MustWinType winHandle) = 0;
    
    virtual gti::GTI_ANALYSIS_RETURN winComplete(MustParallelId pId, MustLocationId lId, MustWinType winHandle) = 0;
    
    virtual gti::GTI_ANALYSIS_RETURN winPost(MustParallelId pId, MustLocationId lId, MustGroupType groupHandle, MustWinType winHandle) = 0;
    
    virtual gti::GTI_ANALYSIS_RETURN winWait(MustParallelId pId, MustLocationId lId, MustWinType winHandle) = 0;

    /**
     * Can't save source and tag here because of wildcards.
     *
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN
    addPersistentRecvInfo(MustParallelId pId, MustRequestType request, MustCommType comm) = 0;
}; /*class I_VectorClockWrapper*/

#endif /*I_VECTORCLOCKWRAPPER_H */
