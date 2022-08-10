/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file WinTrack.h
 *       @see MUST::WinTrack.
 *
 *  @date 26.04.2017
 *  @author Tobias Hilbrich, Simon Schwitanski
 */

#include "ModuleBase.h"
#include "I_WinTrack.h"
#include "TrackBase.h"
#include "Win.h"
#include "I_DatatypeTrack.h"
#include "I_CommTrack.h"
#include "I_BaseConstants.h"
#include "ResourceApi.h"

#ifndef WINTRACK_H
#define WINTRACK_H

using namespace gti;

namespace must
{
	/**
	 * Enumeration of all predefined windows (none).
	 */
	enum MustMpiWinPredefined
	{
		MUST_MPI_WIN_UNKNOWN = 0
	};

	/**
     * Implementation of I_WinTrack.
     */
    class WinTrack : public TrackBase<Win, I_Win, MustWinType, MustMpiWinPredefined, WinTrack, I_WinTrack>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		WinTrack (const char* instanceName);

            GTI_ANALYSIS_RETURN addWin(
                    MustParallelId pId,
                    MustLocationId lId,
                    int kind,
                    int memoryModel,
                    MustCommType comm,
                    void* base,
                    int size,
                    int disp_unit,
                    MustWinType win);

            GTI_ANALYSIS_RETURN attachWin(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustAddressType base,
                    int size,
                    MustWinType win);

            GTI_ANALYSIS_RETURN detachWin(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustAddressType base,
                    MustWinType win);

            GTI_ANALYSIS_RETURN freeWin(
                    MustParallelId pId,
                    MustLocationId lId,
                    MustWinType win);

            GTI_ANALYSIS_RETURN addRemoteWin(
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
                    MustLocationId creationLId);

            /**
             * @see I_WinTrack::freeRemoteWin.
             */
            GTI_ANALYSIS_RETURN freeRemoteWin (
                    int rank,
                    MustRemoteIdType remoteId);

            /**
             * @see I_WinTrack::getWin
             */
            I_Win* getWin (
                    MustParallelId pId,
                    MustWinType win);

            /**
             * @see I_WinTrack::getWin
             */
            I_Win* getWin (
                    int rank,
                    MustWinType win);

            /**
             * @see I_WinTrack::getPersistentWin
             */
            I_WinPersistent* getPersistentWin (
                    MustParallelId pId,
                    MustWinType win);

            /**
             * @see I_WinTrack::getPersistentWin
             */
            I_WinPersistent* getPersistentWin (
                    int rank,
                    MustWinType win);

            /**
             * @see I_WinTrack::getRemoteWin
             */
            I_Win* getRemoteWin (
                    MustParallelId pId,
                    MustRemoteIdType remoteId);

            /**
             * @see I_WinTrack::getRemoteWin
             */
            I_Win* getRemoteWin (
                    int rank,
                    MustRemoteIdType remoteId);

            /**
             * @see I_WinTrack::getPersistentRemoteWin
             */
            I_WinPersistent* getPersistentRemoteWin (
                    MustParallelId pId,
                    MustRemoteIdType remoteId);

            /**
             * @see I_WinTrack::getPersistentRemoteWin
             */
            I_WinPersistent* getPersistentRemoteWin (
                    int rank,
                    MustRemoteIdType remoteId);

            /**
             * @see I_WinTrack::getMatchingWin
             */
            MustWinType getMatchingWin (
                    int remoteRank,
                    int targetRank,
                    MustRemoteIdType remoteId);

            /**
             * @see I_WinTrack::passWinAcross
             */
            bool passWinAcross (
                    MustParallelId pId,
                    MustWinType win,
                    int toPlaceId);

            /**
             * @see I_WinTrack::passWinAcross
             */
            bool passWinAcross (
                    int rank,
                    MustWinType win,
                    int toPlaceId);

            /**
             * @see I_WinTrack::passWinAcross
             */
            bool passWinAcross (
                    int rank,
                    I_Win* win,
                    int toPlaceId,
                    MustRemoteIdType *pOutRemoteId);



    		/**
    		 * Destructor.
    		 */
    		virtual ~WinTrack (void);

    protected:

    		I_DatatypeTrack* myDTrack;
    		I_CommTrack* myCTrack;
    		I_BaseConstants* myConsts;

    		//Function pointers for passing requests across
    		//passFreeAcrossP myPassFreeAcrossFunc;
    		//passWinAcrossP myPassWinAcrossFunc;

    		/**
    		 * Implementation of TrackBase::createPredefinedInfo.
    		 */
    		Win* createPredefinedInfo (int value, MustWinType handle);

            /**
             * Add a window handle.
             */
            void addWindow(
                    MUST_WIN_KIND kind,
                    MustParallelId pId,
                    MustLocationId lId,
                    MustCommType comm,
                    MustAddressType base,
                    int size,
                    int disp_unit,
                    MustWinType win,
                    MUST_WIN_MEMORY_MODEL memoryModel);

    private:
            passWinAcrossP myPassWinAcrossFunc;
            passFreeAcrossP myPassFreeWinAcrossFunc;
            /*
             * Helper function to add a memory interval to a window.
             */
            void addMemoryInterval(Win& win, MustAddressType base, MustAddressType size);

            /**
             * Implementation for the different passWinAcross versions.
             */
            bool passWinAcrossInternal (
                    int rank,
                    Win* win,
                    int toPlaceId,
                    MustRemoteIdType *pOutRemoteId,
                    bool hasHandle,
                    MustWinType handle);

    }; /*class WinTrack */
} /*namespace MUST*/

#endif /*WinTrack_H*/
