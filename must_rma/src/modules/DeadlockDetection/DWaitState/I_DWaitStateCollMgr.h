/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_DWaitStateCollMgr.h
 *       @see I_DWaitStateCollMgr.
 *
 *  @date 05.03.2013
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"

#include "I_ChannelId.h"

#ifndef I_DWAITSTATECOLLMGR_H
#define I_DWAITSTATECOLLMGR_H

/**
 * Listens to collectiveActive requests and acknowledges them
 * if DWaitStateCollReduction determines that a request is complete.
 *
 * Dependencies (order as listed):
 * - ParallelIdAnalysis
 * - CommTrack
 *
 */
class I_DWaitStateCollMgr : public gti::I_Module
{
public:

    /**
     * Notification of an complete collectiveActive request.
     * The way DWaitStateCollReduction works cId must be
     * NULL, i.e., it must be a complete wave, if it is not,
     * then an internal error exists.
     *
     * @param pId parallel id of the call site.
     * @param lId location id of the call site.
     * @param comm of the request.
     * @param numTasks in the aggregated request.
     * @param cId of the request.
     * @return @see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN request (
            int isIntercomm,
            unsigned long long contextId,
            int collCommType,
            int localGroupSize,
            int remoteGroupSize,
            int numTasks,
            gti::I_ChannelId* cId) = 0;

};/*class I_DWaitStateCollMgr*/

#endif /*I_DWAITSTATECOLLMGR_H*/
