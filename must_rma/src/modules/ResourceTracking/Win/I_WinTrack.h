/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_WinTrack.h
 *       @see I_WinTrack.
 *
 *  @date 26.04.2017
 *  @author Tobias Hilbrich, Simon Schwitanski
 */

#include "I_Module.h"
#include "GtiEnums.h"

#include "MustEnums.h"
#include "BaseIds.h"
#include "MustTypes.h"

#include "I_Win.h"
#include "I_TrackBase.h"

#include <list>

#ifndef I_WINTRACK_H
#define I_WINTRACK_H

/**
 * Interface for RMA win tracking analysis module.
 *
 * Important: This analysis module only tracks RMA wins,
 * it provides no correctness checking.
 *
 * Dependencies:
 *  - ParallelIdAnalysis
 *  - LocationAnalysis
 *  - DatatypeTrack
 *  - CommTrack
 *  - BaseConstants
 */
class I_WinTrack : public gti::I_Module, public virtual must::I_TrackBase<must::I_Win>
{
public:
    /**
     * Adds a window.
     * @param pId parallel id of the call site
     * @param lId location id of the call site
     * @param kind window flavor
     * @param memoryModel memory model of window (separate or unified)
     * @param comm communicator corresponding to the window
     * @param base pointer to memory of window
     * @param size size of window
     * @param disp_unit displacement unit of window
     * @param win window id
     */
    virtual gti::GTI_ANALYSIS_RETURN addWin(
            MustParallelId pId,
            MustLocationId lId,
            int kind,
            int memoryModel,
            MustCommType comm,
            void* base,
            int size,
            int disp_unit,
            MustWinType win) = 0;

    /**
     * Adds a memory segment to a dynamic window.
     * @param pId parallel id of the call site
     * @param lId location id of the call site
     * @param base start of new memory segment
     * @param size size of new memory segment
     * @param win window id
     */
    virtual gti::GTI_ANALYSIS_RETURN attachWin(
            MustParallelId pId,
            MustLocationId lId,
            MustAddressType base,
            int size,
            MustWinType win) = 0;

    /**
     * Removes a memory segment from a dynamic window.
     * @param pId parallel id of the call site
     * @param lId location id of the call site
     * @param base memory segment to remove
     * @param win window id
     */
    virtual gti::GTI_ANALYSIS_RETURN detachWin(
            MustParallelId pId,
            MustLocationId lId,
            MustAddressType base,
            MustWinType win) = 0;

    /**
     * Removes a window.
     * @param pId parallel id of the call site
     * @param lId location id of the call site
     * @param win window id
     */
    virtual gti::GTI_ANALYSIS_RETURN freeWin(
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win) = 0;

    /**
     * Returns pointer to win information.
     * Is NULL if this is an unknown handle, note that
     * a MPI_WIN_NULL handle returns a valid pointer though.
     *
     * Memory must not be freed and is valid until I_WinTrack
     * receives the next event, if you need the information longer
     * query getPersistentWin instead.
     *
     * @param pId of the win context.
     * @param win to query for.
     * @return information for the given win.
     */
    virtual must::I_Win* getWin (
            MustParallelId pId,
            MustWinType win) = 0;

    /** As I_WinTrack::getWin with rank instead of pid.*/
    virtual must::I_Win* getWin (
            int rank,
            MustWinType win) = 0;

    /**
     * Like I_WinTrack::getWin, though returns a persistent information
     * that is valid until you erase it, i.e.:
     *@code
     I_WinPersistent * winInfo = myWinTrack->getPersistentWin (pId, handle);
     if (winInfo == NULL) return;
     .... //Do something with winInfo
     winInfo->erase(); //Mark as not needed any longer
     *@endcode
     *
     * A reference count mechanism is used to implement this.
     *
     * @param pId of the window context.
     * @param win to query for.
     * @return information for the given window.
     */
    virtual must::I_WinPersistent* getPersistentWin (
            MustParallelId pId,
            MustWinType win) = 0;

    /** As I_WinTrack::getPersistentWin with rank instead of pid.*/
    virtual must::I_WinPersistent* getPersistentWin (
            int rank,
            MustWinType win) = 0;

    /**
     * Adds a window that was passed to this from a different place on this level.
     *
     * For parameter descriptions see must::Win and passWinAcrossP in
     * ResourceApi.h
     *
     * @return @see GTI_ANALYSIS_RETURN
     */
    virtual gti::GTI_ANALYSIS_RETURN addRemoteWin(
        int rank,
        int hasHandle,
        MustWinType winHandle,
        MustRemoteIdType remoteId,
        int kind,
        int memoryModel,
        MustRemoteIdType commId,
        MustAddressType base,
        int dispUnit,
        unsigned long long contextId,
        MustParallelId creationPId,
        MustLocationId creationLId) = 0;

    /**
     * Frees a window that was passed from another place
     * on the same TBON level.
     *
     * @param pId context for the win.
     * @param remoteId that was assigned to the win on the remote side.
     * @return @see GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN freeRemoteWin (
            int rank,
            MustRemoteIdType remoteId) = 0;

    /**
     * Like I_WinTrack::getWin, but
     * with a remote id instead of a handle.
     */
    virtual must::I_Win* getRemoteWin (
                MustParallelId pId,
                MustRemoteIdType remoteId) = 0;

    /**
     * Like I_WinTrack::getWin, but
     * with a remote id instead of a handle.
     */
    virtual must::I_Win* getRemoteWin (
            int rank,
            MustRemoteIdType remoteId) = 0;

    /**
     * Like I_WinTrack::getPersistentWin, but
     * with a remote id instead of a handle.
     */
    virtual must::I_WinPersistent* getPersistentRemoteWin (
            MustParallelId pId,
            MustRemoteIdType remoteId) = 0;

    /**
     * Like I_WinTrack::getPersistentWin, but
     * with a remote id instead of a handle.
     */
    virtual must::I_WinPersistent* getPersistentRemoteWin (
            int rank,
            MustRemoteIdType remoteId) = 0;

    /**
     * Returns the local window handle which belongs to a remote win.
     * This function iterates through all user handles and is thus expensive.
     * @param remoteRank rank the remote win comes from
     * @param targetRank target rank of the local win
     * @param remoteId that was assigned to the win on the remote side.
     * @return window id matching the remote id window
     */
    virtual MustWinType getMatchingWin (
            int remoteRank,
            int targetRank,
            MustRemoteIdType remoteId) = 0;

    /**
     * Returns a list of all currently known user handles.
     * Usage scenarios involve logging lost handles at finalize.
     * @return a list of pairs of the form (rank, handle id).
     */
    virtual std::list<std::pair<int, MustWinType> > getUserHandles (void) = 0;

    /**
     * Passes the given win to the given place on this tool level.
     * @param pId context of the win to pass
     * @param win to pass
     * @param toPlaceId place to send to
     * @return true iff successful.
     *
     * Reasons for this to fail include the unavailability of intra layer
     * communication.
     */
    virtual bool passWinAcross (
            MustParallelId pId,
            MustWinType win,
            int toPlaceId) = 0;

    /**
     * Like the other passWinAcross version but with rank instead of a pId.
     */
    virtual bool passWinAcross (
            int rank,
            MustWinType win,
            int toPlaceId) = 0;

    /**
     * Like the other passWinAcross versions but with
     * a win info instead of a handle.
     *
     * This is usually more expensive than the other passWinAcross
     * versions as this requires that the tracker checks whether there
     * also exists a handle for this resource. To do that it has to
     * search though all its handles which may be expensive.
     *
     * @param pOutRemoteId pointer to storage for a remote id, is set to
     *               the remote id that is used to identify the resource on the
     *               remote side.
     */
    virtual bool passWinAcross (
            int rank,
            must::I_Win* win,
            int toPlaceId,
            MustRemoteIdType *pOutRemoteId) = 0;



    virtual bool isPredefined(must::I_Win* info) { return false; }

}; /*class I_WinTrack*/

#endif /*I_WINTRACK_H*/
