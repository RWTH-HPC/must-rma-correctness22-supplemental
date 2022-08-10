/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_MPIVectorClock.h
 *       @see I_MPIVectorClock
 *
 *  @date 02.06.2017
 *  @author Simon Schwitanski
 */

#include "GtiEnums.h"

#include "MustEnums.h"
#include "BaseIds.h"
#include "MustTypes.h"

#ifndef I_MPIVECTORCLOCK_H
#define I_MPIVECTORCLOCK_H


/**
 * MPIVectorClock for correctness checks interface implementation.
 *
 * Dependencies (in listed order):
 * - ParallelIdAnalysis
 * - CreateMessage
 * - ArgumentAnalysis
 * - DatatypeTrack
 * - RequestTrack
 * - WinTrack
 */
class I_MPIVectorClock : public gti::I_Module
{
public:
    /**
     * Initializes the vector clock according to the number
     * of MPI processes and sets all clock entries to value 0.
     * @param pId parallel id associated with this call
     */
    virtual gti::GTI_ANALYSIS_RETURN init(
            MustParallelId pId)=0;

    /**
     * Advances the vector clock entry of this process one
     * time step further.
     */
    virtual gti::GTI_ANALYSIS_RETURN tick()=0;

    /**
     * Increments the counter of pending RMA calls to the given
     * target rank.
     *
     * @targetRank rank the RMA call is addressed to
     */
    virtual void addPendingRMACall(int targetRank)=0;

    /**
     * Called when a new RMA call has arrived. Decrements
     * the counter used for waiting for a certain number
     * of RMA calls.
     */
    virtual gti::GTI_ANALYSIS_RETURN receiveRMACall()=0;

    /**
     * Returns the current vector clock associated with this process.
     * @return current vector clock
     */
    virtual const std::vector<unsigned long long>& get()=0;

    /**
      Returns the current vector clock entry of this process.
     * @return current vector clock entry of this process
     */
    virtual unsigned long long getLocalEntry()=0;

    /**
     * Collective call used as synchronous communication for
     * vector clock.
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param comm communicator used in this call
     * @param hasRequest true iff collective is request-based
     * @param request request type if collective is request-based
     * @param ann annotation address for TSan annotations
     */
    virtual gti::GTI_ANALYSIS_RETURN coll(
            MustParallelId pId,
            MustLocationId lId,
            MustCommType comm,
            int hasRequest,
            MustRequestType request,
            void* ann
            )=0;

    /**
     * The vector clock is sent to the given target rank using
     * PMPI_Send.
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param dest rank of target the vector clock is sent to
     * @param comm communicator used in this call
     * @param ann annotation address for TSan annotations
     */
    virtual gti::GTI_ANALYSIS_RETURN send(
            MustParallelId pId,
            MustLocationId lId,
            int dest,
            MustCommType comm,
            void* ann
            )=0;

    /**
     * Waits for the vector clock of the given source rank using
     * PMPI_Recv.
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param source rank of source the vector clock is received from
     * @param comm communicator used in this call
     * @param ann annotation address for TSan annotations
     */
    virtual gti::GTI_ANALYSIS_RETURN recv(
            MustParallelId pId,
            MustLocationId lId,
            int source,
            MustCommType comm,
            void* ann
            )=0;

    /**
     * Combines send and recv of vector clock in a singel call using
     * PMPI_Sendrecv.
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param dest rank of target the vector clock is send to 
     * @param source rank of source the vector clock is received from
     * @param comm communicator used in this call
     * @param ann annotation address for TSan annotations
     */
    virtual gti::GTI_ANALYSIS_RETURN sendRecv(
            MustParallelId pId,
            MustLocationId lId,
            int dest,
            int source,
            MustCommType comm,
            void* ann
            )=0;

    /**
     * The vector clock is sent to the given target rank using
     * PMPI_Isend.
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param dest rank of target the vector clock is sent to
     * @param comm communicator used in this call
     * @param ann annotation address for TSan annotations
     */
    virtual gti::GTI_ANALYSIS_RETURN isend(
            MustParallelId pId,
            MustLocationId lId,
            int dest,
            MustCommType comm,
            MustRequestType request,
            void* ann
            )=0;

    /**
     * Initiates receive for the vector clock of the given source rank using
     * PMPI_Irecv.
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param source rank of source the vector clock is received from
     * @param comm communicator used in this call
     * @param ann annotation address for TSan annotations
     */
    virtual gti::GTI_ANALYSIS_RETURN irecv(
            MustParallelId pId,
            MustLocationId lId,
            int source,
            MustCommType comm,
            MustRequestType request
            )=0;

    /**
     * Called from AppThrAnn module if the request of an Isend operation
     * has completed.
     *
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param request completed request
     */
    virtual gti::GTI_ANALYSIS_RETURN completedIsend(
            MustParallelId pId,
            MustLocationId lId,
            MustRequestType request
            )=0;

    /**
     * Called from AppThrAnn module if the request of an Irecv operation
     * has completed.
     *
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param source source rank of received message
     * @param request completed request
     * @param ann TSan annoation address
     */
    virtual gti::GTI_ANALYSIS_RETURN completedIrecv(
            MustParallelId pId,
            MustLocationId lId,
            int source,
            MustRequestType request,
            void* ann
            )=0;

    /**
     * Called from AppThrAnn module if the request of a non-blocking collective operation
     * has completed.
     *
     * @param pId parallel id associated with this call
     * @param lId location id associated with this call
     * @param request completed request
     */
    virtual gti::GTI_ANALYSIS_RETURN completedIcoll(
            MustParallelId pId,
            MustLocationId lId,
            MustRequestType request,
            void* ann
            )=0;

    /**
     * Returns the TSan sync clock address of the given MPI clock value.
     *
     * @param clock clock value
     */
    virtual void* getTSanSyncClock(int clock)=0;

    /**
     * Performs the TSan annotations to synchronize with an application thread.
     * Annotates a happens-before arc from application thread to tool thread.
     * @param ann TSan annotation address from application thread
     */
    virtual gti::GTI_ANALYSIS_RETURN happensBeforeSync(void* ann)=0;

    /**
     * Performs the TSan annotations to synchronize with an application thread.
     * Annotates a happens-before arc from tool thread to application thread.
     * @param ann TSan annotation address from application thread
     */
    virtual gti::GTI_ANALYSIS_RETURN happensAfterSync(void* ann)=0;


}; /*class I_MPIVectorClock*/

#endif /*I_MPIVECTORCLOCK_H*/
