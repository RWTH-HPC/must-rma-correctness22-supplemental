/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_AppThrAnn.h
 *       @see I_AppThrAnn
 *
 *  @date 27.06.2017
 *  @author Simon Schwitanski
 */

#include "GtiEnums.h"

#include "MustEnums.h"
#include "BaseIds.h"
#include "MustTypes.h"

#ifndef I_APPTHRANN_H
#define I_APPTHRANN_H


/**
 * Annotates happens-before and happens-after relations at RMA calls
 * and passes the calls to other interested modules (e.g. RMATrack).
 *
 * Dependencies (in listed order):
 * - ParallelIdAnalysis
 * - BaseConstants
 * - WinTrack
 * - TSan
 */
class I_AppThrAnn : public gti::I_Module
{
public:
    /**
     * Adds a get RMA communication call.
     * @param pId parallel context
     * @param lId location id of context.
     * @param originaddr address of the buffer in which to receive the data
     * @param origincount number of entries in origin buffer
     * @param origintype datatpye of each entry in origin buffer
     * @param target rank of target
     * @param targetdisp displacement from window start of the beginning of the target buffer
     * @param targetcount number of entries in target buffer
     * @param targettype datatype of each entry in target buffer
     * @param win window object used for communication
     * @param requesttype request associated with RMA call (optional, set to 0 if not used)
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN addGetOperation (
            MustParallelId pId,
            MustLocationId lId,
            MustAddressType originaddr,
            int origincount,
            MustDatatypeType origintype,
            int target,
            int targetdisp,
            int targetcount,
            MustDatatypeType targettype,
            MustWinType win,
            MustRequestType requesttype
            )=0;

    /**
     * Adds a put RMA communication call.
     * @param pId parallel context
     * @param lId location id of context.
     * @param originaddr address of the buffer of which to read the data
     * @param origincount number of entries in origin buffer
     * @param origintype datatpye of each entry in origin buffer
     * @param target rank of target
     * @param targetdisp displacement from window start of the beginning of the target buffer
     * @param targetcount number of entries in target buffer
     * @param targettype datatype of each entry in target buffer
     * @param win window object used for communication
     * @param requesttype request associated with RMA call (optional, set to 0 if not used)
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN addPutOperation (
            MustParallelId pId,
            MustLocationId lId,
            MustAddressType originaddr,
            int origincount,
            MustDatatypeType origintype,
            int target,
            int targetdisp,
            int targetcount,
            MustDatatypeType targettype,
            MustWinType win,
            MustRequestType requesttype
            )=0;

    /**
     * Adds an accumulate RMA communication call.
     * @param pId parallel context
     * @param lId location id of context.
     * @param originaddr address of the buffer of which to read the data
     * @param origincount number of entries in origin buffer
     * @param origintype datatpye of each entry in origin buffer
     * @param target rank of target
     * @param targetdisp displacement from window start of the beginning of the target buffer
     * @param targetcount number of entries in target buffer
     * @param targettype datatype of each entry in target buffer
     * @param win window object used for communication
     * @param requesttype request associated with RMA call (optional, set to 0 if not used)
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN addAccumulateOperation (
            MustParallelId pId,
            MustLocationId lId,
            MustAddressType originaddr,
            int origincount,
            MustDatatypeType origintype,
            int target,
            int targetdisp,
            int targetcount,
            MustDatatypeType targettype,
            MustOpType op,
            MustWinType win,
            MustRequestType requesttype
            )=0;

    /**
     * Adds a get accumulate RMA communication call.
     * @param pId parallel context
     * @param lId location id of context.
     * @param originaddr address of the buffer of which to read data
     * @param origincount number of entries in origin buffer
     * @param origintype datatype of each entry in origin buffer
     * @param resultaddr address of result buffer
     * @param resultcount number of entries in result buffer
     * @param resulttype datatype of each entry in result buffer
     * @param target rank of target
     * @param targetdisp displacement from window start of the beginning of the target buffer
     * @param targetcount number of entries in target buffer
     * @param targettype datatype of each entry in target buffer
     * @param win window object used for communication
     * @param requesttype request associated with RMA call (optional, set to 0 if not used)
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN addGetAccumulateOperation (
            MustParallelId pId,
            MustLocationId lId,
            MustAddressType originaddr,
            int origincount,
            MustDatatypeType origintype,
            MustAddressType resultaddr,
            int resultcount,
            MustDatatypeType resulttype,
            int target,
            int targetdisp,
            int targetcount,
            MustDatatypeType targettype,
            MustOpType op,
            MustWinType win,
            MustRequestType requesttype
            )=0;

    virtual gti::GTI_ANALYSIS_RETURN winCreate (
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winLock (
            MustParallelId pId,
            MustLocationId lId,
            int lock_type,
            int rank,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winSync (
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winUnlock (
            MustParallelId pId,
            MustLocationId lId,
            int rank,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winUnlockAll (
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winFence (
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winStart (
            MustParallelId pId,
            MustLocationId lId,
            MustGroupType group,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winComplete (
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winPost (
            MustParallelId pId,
            MustLocationId lId,
            MustGroupType group,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winWait (
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winFlush (
            MustParallelId pId,
            MustLocationId lId,
            int rank,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winFlushLocal (
            MustParallelId pId,
            MustLocationId lId,
            int rank,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winFlushAll (
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN winFlushLocalAll (
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win)=0;

    virtual gti::GTI_ANALYSIS_RETURN send(
            MustParallelId pId,
            MustLocationId lId,
            int dest,
            MustCommType comm
            )=0;

    virtual gti::GTI_ANALYSIS_RETURN recv(
            MustParallelId pId,
            MustLocationId lId,
            int source,
            MustCommType comm
            )=0;

    virtual gti::GTI_ANALYSIS_RETURN sendRecv(
            MustParallelId pId,
            MustLocationId lId,
            int dest,
            int source,
            MustCommType comm
            )=0;

    virtual gti::GTI_ANALYSIS_RETURN isend(
            MustParallelId pId,
            MustLocationId lId,
            int dest,
            MustCommType comm,
            MustRequestType request
            )=0;

    virtual gti::GTI_ANALYSIS_RETURN irecv(
            MustParallelId pId,
            MustLocationId lId,
            int source,
            MustCommType comm,
            MustRequestType request
            )=0;

    virtual gti::GTI_ANALYSIS_RETURN coll(
            MustParallelId pId,
            MustLocationId lId,
            MustCommType comm,
            int hasRequest,
            MustRequestType request
            )=0;

    virtual gti::GTI_ANALYSIS_RETURN completedRequest (
            MustParallelId pId,
            MustLocationId lId,
            int source,
            MustRequestType request)=0;

    virtual gti::GTI_ANALYSIS_RETURN completedRequests (
            MustParallelId pId,
            MustLocationId lId,
            int* sources,
            MustRequestType *requests,
            int count)=0;

    virtual gti::GTI_ANALYSIS_RETURN annotateSignal ()=0;

}; /*class I_AppThrAnn*/

#endif /*I_APPTHRANN_H*/
