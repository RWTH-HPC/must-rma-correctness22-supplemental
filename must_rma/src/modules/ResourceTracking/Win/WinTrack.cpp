/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file WinTrack.cpp
 *       @see MUST::WinTrack.
 *
 *  @date 26.04.2017
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"

#include "WinTrack.h"

#include <sstream>

using namespace must;

mGET_INSTANCE_FUNCTION(WinTrack)
mFREE_INSTANCE_FUNCTION(WinTrack)
mPNMPI_REGISTRATIONPOINT_FUNCTION(WinTrack)

//=============================
// Constructor
//=============================
WinTrack::WinTrack (const char* instanceName)
	: TrackBase<Win, I_Win, MustWinType, MustMpiWinPredefined, WinTrack, I_WinTrack> (instanceName)
{
    //Get the DatatypeTrack and CommTrack modules
    if (myFurtherMods.size () < 3)
    {
        std::cerr << "Error: the WinTrack module needs the DatatypeTrack and CommTrack modules as childs, but at least one of them was not available." << std::endl;
        assert (0);
    }

    myDTrack = (I_DatatypeTrack*) myFurtherMods[0];
    myCTrack = (I_CommTrack*) myFurtherMods[1];
    myConsts = (I_BaseConstants*) myFurtherMods[2];

    //Initialize module data
    getWrapAcrossFunction("passWinAcross",(GTI_Fct_t*) &myPassWinAcrossFunc);
    getWrapAcrossFunction("passFreeWinAcross",(GTI_Fct_t*) &myPassFreeWinAcrossFunc);
}

//=============================
// freeWin
//=============================
GTI_ANALYSIS_RETURN WinTrack::freeWin(
            MustParallelId pId,
            MustLocationId lId,
            MustWinType win) {

    // find window
    Win* info = getHandleInfo(pId, win);
    if (!info) return GTI_ANALYSIS_SUCCESS;

    // remove window
    // TODO: we cannot remove the handle here, because we cannot be sure that the
    // window information will not be needed anywhere
    //removeUserHandle(pId, win);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// attachWin
//=============================
GTI_ANALYSIS_RETURN WinTrack::attachWin(
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType base,
        int size,
        MustWinType win) {

    // find window
    Win* info = getHandleInfo(pId, win);
    if (!info) return GTI_ANALYSIS_SUCCESS;
    addMemoryInterval(*info, (MustAddressType) base, size);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// detachWin
//=============================
GTI_ANALYSIS_RETURN WinTrack::detachWin(
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType base,
        MustWinType win) {

    // find window
    Win* info = getHandleInfo(pId, win);
    if (!info) return GTI_ANALYSIS_SUCCESS;

    // search for memory block and remove it (O(n), efficiency?)
    for (MustMemIntervalListType::iterator it = info->myMemIntervals.begin(); it != info->myMemIntervals.end(); ++it) {
        if (it->baseAddress == base) {
            info->myMemIntervals.erase(it);
            break;
        }
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// createPredefinedInfo
//=============================
Win* WinTrack::createPredefinedInfo (int value, MustWinType handle)
{
    if (handle == myNullValue)
        return new Win ();
    return NULL;//There should not be any other cases
}

//============================
// addWin
//============================
GTI_ANALYSIS_RETURN WinTrack::addWin(
        MustParallelId pId,
        MustLocationId lId,
        int kind,
        int memoryModel,
        MustCommType comm,
        void* base,
        int size,
        int disp_unit,
        MustWinType win) {

    Win* info = new Win ();
    info->myKind = (MUST_WIN_KIND) kind;
    info->myMemoryModel = (MUST_WIN_MEMORY_MODEL) memoryModel;
    info->myCreationPId = pId;
    info->myCreationLId = lId;
    info->myComm = myCTrack->getPersistentComm(pId, comm);
    info->myCommHandle = comm;
    info->myContextId = info->myComm->getNextContextId();

    if (info->myKind == MUST_WIN_DYNAMIC) {
        // dynamic window have MPI_BOTTOM as base address
        info->myBase = (MustAddressType) myConsts->getBottom();
    } else {
        info->myBase = (MustAddressType) base;
    }

    info->myDispUnit = disp_unit;

#ifdef MUST_DEBUG
    if (info->myMemoryModel == MUST_WIN_MEMORY_UNKNOWN)
        std::cout << "Warning: Memory model of RMA window is unknown!" << std::endl;
    std::cout << "window base: " << info->myBase << std::endl;
#endif

    // add memory region corresponding to window
    if (info->myKind != MUST_WIN_DYNAMIC) {
        addMemoryInterval(*info, info->myBase, size);
    }
     
    submitUserHandle(pId, win, info);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addMemoryInterval
//=============================
void WinTrack::addMemoryInterval(Win& win, MustAddressType base, MustAddressType size)
{
    win.myMemIntervals.insert(memInterval(
        StridedBlock(base, base, true, 0, 1, size, 0), base, (MustRequestType) 0, true, NULL, base, 0));
}

//=============================
// addRemoteWin
//=============================
GTI_ANALYSIS_RETURN WinTrack::addRemoteWin(
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
                    MustLocationId creationLId) {
    // create new win
    Win* info = new Win ();
    info->myKind = (MUST_WIN_KIND) kind;
    info->myMemoryModel = (MUST_WIN_MEMORY_MODEL) memoryModel;
    info->myComm = myCTrack->getPersistentRemoteComm(rank, commId);
    info->myCommHandle = 0;
    info->myCreationPId = creationPId;
    info->myCreationLId = creationLId;
    info->myContextId = contextId;
    info->myBase = base;
    info->myDispUnit = dispUnit;

    // register the new remote win
    submitRemoteResource(rank, remoteId, hasHandle, winHandle, info);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// freeRemoteWin
//=============================
GTI_ANALYSIS_RETURN WinTrack::freeRemoteWin (
        int rank,
        MustRemoteIdType remoteId)
{
    removeRemoteResource(rank, remoteId);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// getWin
//=============================
I_Win* WinTrack::getWin (
		MustParallelId pId,
		MustWinType win)
{
    return getWin (pId2Rank (pId), win);
}

//=============================
// getWin
//=============================
I_Win* WinTrack::getWin (
		int rank,
		MustWinType win)
{
    return getHandleInfo (rank, win);
}

//=============================
// getPersistentWin
//=============================
I_WinPersistent* WinTrack::getPersistentWin (
        MustParallelId pId,
        MustWinType win)
{
    return getPersistentWin (pId2Rank (pId), win);
}

//=============================
// getPersistentWin
//=============================
I_WinPersistent* WinTrack::getPersistentWin (
        int rank,
        MustWinType win)
{
    Win* ret = getHandleInfo (rank, win);;
    if (ret) ret->incRefCount();
    return ret;
}

//=============================
// getRemoteWin
//=============================
I_Win* WinTrack::getRemoteWin (
        MustParallelId pId,
        MustRemoteIdType remoteId)
{
    return getRemoteWin (pId2Rank (pId), remoteId);
}

//=============================
// getRemoteWin
//=============================
I_Win* WinTrack::getRemoteWin (
        int rank,
        MustRemoteIdType remoteId)
{
    Win* ret = getRemoteIdInfo (rank,remoteId);
    return ret;
}

//=============================
// getPersistentRemoteWin
//=============================
I_WinPersistent* WinTrack::getPersistentRemoteWin (
        MustParallelId pId,
        MustRemoteIdType remoteId)
{
    return getPersistentRemoteWin (pId2Rank (pId), remoteId);
}

//=============================
// getPersistentRemoteWin
//=============================
I_WinPersistent* WinTrack::getPersistentRemoteWin (
        int rank,
        MustRemoteIdType remoteId)
{
    Win* ret = getRemoteIdInfo (rank,remoteId);
    if (ret) ret->incRefCount();
    return ret;
}

MustWinType WinTrack::getMatchingWin (
        int remoteRank,
        int targetRank,
        MustRemoteIdType remoteId) {

    I_Win* remoteWin = getRemoteWin(remoteRank, remoteId);

    std::list<std::pair<int, MustWinType> > handles = getUserHandles();
    for (std::list<std::pair<int, MustWinType> >::iterator it = handles.begin();
            it != handles.end(); ++it) {
        if (it->first == targetRank && *remoteWin == *getWin(it->first, it->second))
            return it->second;
    }

    assert(0);
    return 0;
}

//=============================
// passWinAcross
//=============================
bool WinTrack::passWinAcross (
        MustParallelId pId,
        MustWinType win,
        int toPlaceId)
{
    return passWinAcross (pId2Rank(pId), win, toPlaceId);
}

//=============================
// passWinAcross
//=============================
bool WinTrack::passWinAcross (
        int rank,
        MustWinType winHandle,
        int toPlaceId)
{
    //Get win
    Win* win = getHandleInfo (rank, winHandle);

    //Use the existing passWinAcross
    return passWinAcrossInternal (rank, win, toPlaceId, NULL, true, winHandle);
}

//=============================
// passWinAcross
//=============================
bool WinTrack::passWinAcross (
        int rank,
        I_Win* winIn,
        int toPlaceId,
        MustRemoteIdType *pOutRemoteId)
{
    if (!winIn)
        return false; //Invalid win

    //Cast to internal representation
    Win* win = (Win*) winIn;

    //Do we still have a handle associated?
    MustWinType handle = 0;
    bool hasHandle = getHandleForInfo (rank, win, &handle);

    return passWinAcrossInternal (rank, win, toPlaceId, pOutRemoteId, hasHandle, handle);
}

//=============================
// passWinAcrossInternal
//=============================
bool WinTrack::passWinAcrossInternal (
        int rank,
        Win* win,
        int toPlaceId,
        MustRemoteIdType *pOutRemoteId,
        bool hasHandle,
        MustWinType handle)
{
    //Do we have wrap-across at all?
    if (!myPassWinAcrossFunc)
        return false;

    //Valid info?
    if (!win)
        return false;

    //Store the remote id
    if (pOutRemoteId)
        *pOutRemoteId = win->getRemoteId();

    //Did we already pass this win?
    if (win->wasForwardedToPlace(toPlaceId, rank))
        return true;

    //Pass base resources of the win
    myLIdMod->passLocationToPlace(win->myCreationPId, win->myCreationLId, toPlaceId);

    MustRemoteIdType commId = 0;
    myCTrack->passCommAcross(rank, win->myComm, toPlaceId, &commId);

    //Pass the actual win across
    (*myPassWinAcrossFunc) (
            rank,
            (int) hasHandle,
            handle,
            win->getRemoteId(),
            (int) win->myKind,
            (int) win->myMemoryModel,
            commId,
            win->myBase,
            win->myDispUnit,
            win->myContextId,
            win->myCreationPId,
            win->myCreationLId,
            toPlaceId
    );

    //Tell the win that we passed it across
    win->setForwardedToPlace(toPlaceId, rank, myPassFreeWinAcrossFunc);

    return true;
}


//=============================
// Destructor
//=============================
WinTrack::~WinTrack ()
{
    //Notify HandleInfoBase of ongoing shutdown
    HandleInfoBase::disableFreeForwardingAcross();
    myDTrack->notifyOfShutdown();
    myCTrack->notifyOfShutdown();
}
/*EOF*/
