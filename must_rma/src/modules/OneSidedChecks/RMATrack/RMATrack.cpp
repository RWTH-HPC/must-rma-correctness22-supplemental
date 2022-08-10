/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file RMATrack.cpp
 *       @see must::RMATrack.
 *
 *  @date 22.05.2017
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"
#include "RMATrack.h"
#include "MustEnums.h"
#include "MustDefines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sstream>
#include <fstream>

using namespace must;

mGET_INSTANCE_FUNCTION(RMATrack)
mFREE_INSTANCE_FUNCTION(RMATrack)
mPNMPI_REGISTRATIONPOINT_FUNCTION(RMATrack)

//=============================
// Constructor.
//=============================
RMATrack::RMATrack (const char* instanceName)
    : ModuleBase<RMATrack, I_RMATrack> (instanceName), myRMAId(0)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUBMODULES 12
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

    myPIdMod = (I_ParallelIdAnalysis*) subModInstances[0];
    myLogger = (I_CreateMessage*) subModInstances[1];
    myConsts = (I_BaseConstants*) subModInstances[2];
    myDatMod = (I_DatatypeTrack*) subModInstances[3];
    myReqMod = (I_RequestTrack*) subModInstances[4];
    myGrpMod = (I_GroupTrack*) subModInstances[5];
    myLIdMod = (I_LocationAnalysis*) subModInstances[6];
    myWinMod = (I_WinTrack*) subModInstances[7];
    myTSanMod = (I_TSan*) subModInstances[8];
    myMPIVectorClock = (I_MPIVectorClock*) subModInstances[9];
    myTSanSyncClockRecorder = (I_TSanSyncClockRecorder*) subModInstances[10];
    myVectorClock = (I_VectorClock*) subModInstances[11];

    //Initialize module data
    getWrapAcrossFunction("passTargetRMAOpAcross",(GTI_Fct_t*) &myPassTargetRMAOpAcrossFunc);
    getWrapAcrossFunction("passTargetCompletionAcross",(GTI_Fct_t*) &myPassTargetCompletionAcrossFunc);
    getWrapperFunction("notifyOriginOpStart",(GTI_Fct_t*) &myNotifyOriginOpStartFunc);
    getWrapperFunction("notifyOriginOpComplete",(GTI_Fct_t*) &myNotifyOriginOpCompleteFunc);
    getWrapperFunction("notifyTargetOpStart",(GTI_Fct_t*) &myNotifyTargetOpStartFunc);
    getWrapperFunction("notifyTargetOpComplete",(GTI_Fct_t*) &myNotifyTargetOpCompleteFunc);
}

//=============================
// init
//=============================
GTI_ANALYSIS_RETURN RMATrack::init(MustParallelId pId) {
    // get own rank
    uint64_t rank = myPIdMod->getInfoForId(pId).rank;

    // set RMA id counter accordingly such that we have unique ids across processes
    myRMAId = rank << 32;

    getPlaceMod(&myPlaceMod);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addRemoteRMA
//=============================
GTI_ANALYSIS_RETURN RMATrack::addRemoteRMA (
                int origin,
                MustRMAId rmaId,
                MustParallelId pId,
                MustLocationId lId,
                bool isStore,
                bool isAtomic,
                bool isLocked,
                int target,
                int disp,
                int count,
                MustRemoteIdType originDatatypeId,
                MustRemoteIdType targetDatatypeId,
                MustRemoteIdType winId,
                int epoch,
                int accessMode,
                unsigned long long originClock,
                void* returnAddr,
                void* functionAddr) {
    
#ifdef MUST_DEBUG
        std::stringstream msg;
        msg << "[RMATrack] Add remote RMA"
                  << "origin=" << origin
                  << ",rmaId=" << rmaId
                  << ",pId=" << pId
                  << ",lId=" << lId
                  << ",isStore=" << isStore
                  << ",isAtomic=" << isAtomic
                  << ",isLocked=" << isLocked
                  << ",target=" << target
                  << ",disp=" << disp
                  << ",count=" << count
                  << ",originDatatypeId=" << originDatatypeId
                  << ",targetDatatypeId=" << targetDatatypeId
                  << ",winId=" << winId
                  << ",epoch=" << epoch
                  << ",accessMode=" << accessMode
                  << ",originClock=" << originClock
                  << ",returnAddr=" << returnAddr
                  << ",functionAddr=" << functionAddr
                  << std::endl;
        std::cout << msg.str();
#endif
    I_Datatype* originDatatype;
    I_DatatypePersistent* targetDatatype;
    I_WinPersistent* localWin;
    MustMemIntervalListType intervalList;
    MustWinType localWinId = myWinMod->getMatchingWin(origin, target, winId);

    // create the RMA call
    if (originDatatypeId)
        originDatatype = myDatMod->getPersistentRemoteDatatype(origin, originDatatypeId);
    else
        originDatatype = NULL;

    if (targetDatatypeId)
        targetDatatype = myDatMod->getPersistentRemoteDatatype(origin, targetDatatypeId);
    else
        targetDatatype = NULL;

    if (localWinId) {
        // look for matching local win here
        localWin = myWinMod->getPersistentWin(target, localWinId);
    } else {
        localWin = NULL;
        assert(0);
    }


    void* annAddr;
    if (originClock == 0) {
        // Special case: If origin clock is 0, both processes never synchronized, so we take the window
        // creation function as start of the concurrent region.
        annAddr = myWinCreateAddrs[localWinId];
    } else {
        annAddr = myTSanSyncClockRecorder->getSyncClock(originClock);
    }

    addTargetOp(
            origin,
            rmaId,
            pId,
            lId,
            isStore,
            isAtomic,
            isLocked,
            target,
            disp,
            count,
            originDatatype,
            targetDatatype,
            localWinId,
            epoch,
            accessMode,
            annAddr,
            returnAddr,
            functionAddr
    );

    return GTI_ANALYSIS_SUCCESS;
}



//=============================
// addRemoteTargetCompletion
//=============================
GTI_ANALYSIS_RETURN RMATrack::addRemoteTargetCompletion (
                MustParallelId pId,
                MustLocationId lId,
                int origin,
                int target,
                MustRemoteIdType winId,
                int epoch) {
    MustWinType localWinId;

    if (origin == target) {
        localWinId = winId; // directly transmitted winId, no matching win has to be found
    } else {
        localWinId = myWinMod->getMatchingWin(origin, target, winId);
    }

    if (epoch != -1) { // general active target completion
        // update remote epoch counter
        auto it = myRMARemoteActiveEpochCounter.find(localWinId);
        if (it != myRMARemoteActiveEpochCounter.end()) {
            myRMARemoteActiveEpochCounter[localWinId][origin] = epoch;
        } else {
            std::vector<int>epochCounter(myWinMod->getWin(pId, localWinId)->getComm()->getGroup()->getSize(), 0);
            epochCounter[origin] = epoch;
            myRMARemoteActiveEpochCounter[localWinId] = epochCounter;
        }
    } else { // passive target completion
        // check whether myTgtStateMap[winId][origin] exists
        if (myTgtStateMap[localWinId].find(origin) != myTgtStateMap[localWinId].end()) {
            // transition INIT -> CONSISTENT
            // get all items in state INIT and CONSISTENT
            std::list<MustRMAId>& initItems = myTgtStateMap[localWinId][origin][TARGET_OP_STATE_INIT];
            std::list<MustRMAId>& consistentItems = myTgtStateMap[localWinId][origin][TARGET_OP_STATE_CONSISTENT];

            I_Win* win = myWinMod->getWin(pId, localWinId);
            if (origin != target) {
                // move items from INIT to CONSISTENT
                consistentItems.splice(consistentItems.end(), initItems);
            } else if (origin == target) {
                // special case: process accesses its own local window, we can directly
                // annotate those RMA operations and do not have to wait for any
                // process synchronization call
                notifyTargetOpComplete(0, 0, initItems);
                // remove the RMA call information
                myTgtRMAMap.removeOps(initItems);
                // remove the RMA ids from the state map
                initItems.clear();
            } else {
                assert(0);
            }
        }
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// receiveSync
//=============================
GTI_ANALYSIS_RETURN RMATrack::receiveSync (
                int remoteRank,
                int ownRank) {
    std::map<MustWinType, std::map<int, std::vector<std::list<MustRMAId> > > >::iterator winIt;
    std::map<int, std::vector<std::list<MustRMAId> > >::iterator rankIt;

    for (winIt = myTgtStateMap.begin(); winIt != myTgtStateMap.end(); ++winIt) {
        // look whether there are RMA operations of remoteRank pending (passive)
        rankIt = winIt->second.find(remoteRank);
        if (rankIt != winIt->second.end()) {
            // pending RMA operations found
            std::list<MustRMAId>& consistentItems = rankIt->second[TARGET_OP_STATE_CONSISTENT];
            if (consistentItems.size() == 0)
                continue;

            I_Win* win = myWinMod->getWin(ownRank, winIt->first);
            if (win->getMemoryModel() == MUST_WIN_MEMORY_SEPARATE) {
                // transition CONSISTENT -> CONSISTENT_HB
                std::list<MustRMAId>& consistentHbItems = rankIt->second[TARGET_OP_STATE_CONSISTENT_HB];
                consistentHbItems.splice(consistentHbItems.end(), consistentItems);
            } else if (win->getMemoryModel() == MUST_WIN_MEMORY_UNIFIED) {
                // consistent items can be annotated
                notifyTargetOpComplete(0, 0, consistentItems);
                // remove the RMA call information
                myTgtRMAMap.removeOps(consistentItems);
                // remove the RMA ids from the state map
                consistentItems.clear();
            }
        }
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addGetOperation
//=============================
GTI_ANALYSIS_RETURN RMATrack::addGetOperation (
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
                MustRequestType requesttype,
                void* ann,
                void* functionaddr
                ) {
    // check for MPI_PROC_NULL
    if (myConsts->isProcNull(target))
        return GTI_ANALYSIS_SUCCESS;

    const void* returnaddr = myLIdMod->getInfoForId(pId, lId).codeptr;

    // translate target rank to world comm
    int realTarget = translateRank(myWinMod->getWin(pId, win)->getComm(), target);

    // get a unique Id for the call such that we can match origin and target memory operations
    MustRMAId callId = nextId();
    addOriginOp(callId, pId, lId, true, originaddr, origincount, origintype, realTarget, win, requesttype, ann, (void*) returnaddr, functionaddr);

    int levelId;
    getLevelIdForApplicationRank(realTarget, &levelId);
    int origin = myPIdMod->getInfoForId(pId).rank;
#ifdef MUST_DEBUG
    std::cout << "passTargetRMAOpAcross("<<origin<<", "<<origin<<", "<<callId<<", "<<pId<<", "<<lId<<", "<<false<<", "<<false<<", "
            <<realTarget<<", "<<targetdisp<<", "<<targetcount<<", "<<origintype<<", "<<targettype<<", "<<win<<", "<<levelId<<", "<<returnaddr<<", "<<functionaddr<<")"<<std::endl;
#endif
    passTargetRMAOpAcross(origin, origin, callId, pId, lId, false, false,
            realTarget, targetdisp, targetcount, origintype, targettype, win, (void*) returnaddr, functionaddr, levelId);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addPutOperation
//=============================
GTI_ANALYSIS_RETURN RMATrack::addPutOperation (
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
                MustRequestType requesttype,
                void* ann,
                void* functionaddr
                ) {
    // check for MPI_PROC_NULL
    if (myConsts->isProcNull(target))
        return GTI_ANALYSIS_SUCCESS;

    const void* returnaddr = myLIdMod->getInfoForId(pId, lId).codeptr;

    // translate target rank to world comm
    int realTarget = translateRank(myWinMod->getWin(pId, win)->getComm(), target);

    // get a unique Id for the call such that we can match origin and target memory operations
    MustRMAId callId = nextId();
    addOriginOp(callId, pId, lId, false, originaddr, origincount, origintype, realTarget, win, requesttype, ann, (void*) returnaddr, functionaddr);

    int levelId;
    getLevelIdForApplicationRank(realTarget, &levelId);
    int origin = myPIdMod->getInfoForId(pId).rank;
#ifdef MUST_DEBUG
    std::cout << "passTargetRMAOpAcross("<<origin<<", "<<origin<<", "<<callId<<", "<<pId<<", "<<lId<<", "<<false<<", "<<false<<", "
            <<realTarget<<", "<<targetdisp<<", "<<targetcount<<", "<<origintype<<", "<<targettype<<", "<<win<<", "<<levelId<<", "<<returnaddr<<", "<<functionaddr<<")"<<std::endl;
#endif
    passTargetRMAOpAcross(origin, origin, callId, pId, lId, true, false,
            realTarget, targetdisp, origincount, origintype, targettype, win, (void*) returnaddr, functionaddr, levelId);


    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addAccumulateOperation
//=============================
GTI_ANALYSIS_RETURN RMATrack::addAccumulateOperation (
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
                MustRequestType requesttype,
                void* ann,
                void* functionaddr
                ) {
    // check for MPI_PROC_NULL
    if (myConsts->isProcNull(target))
        return GTI_ANALYSIS_SUCCESS;
    
    const void* returnaddr = myLIdMod->getInfoForId(pId, lId).codeptr;

    // translate target rank to world comm
    int realTarget = translateRank(myWinMod->getWin(pId, win)->getComm(), target);

    // get a unique Id for the call such that we can match origin and target memory operations
    MustRMAId callId = nextId();
    addOriginOp(callId, pId, lId, false, originaddr, origincount, origintype, realTarget, win, requesttype, ann, (void*) returnaddr, functionaddr);

    int levelId;
    getLevelIdForApplicationRank(realTarget, &levelId);
    int origin = myPIdMod->getInfoForId(pId).rank;
    passTargetRMAOpAcross(origin, origin, callId, pId, lId, true, true,
            realTarget, targetdisp, origincount, origintype, targettype, win, (void*) returnaddr, functionaddr, levelId);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// addGetAccumulateOperation
//=============================
GTI_ANALYSIS_RETURN RMATrack::addGetAccumulateOperation (
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
                MustRequestType requesttype,
                void* ann,
                void* functionaddr
                ) {
    // check for MPI_PROC_NULL
    if (myConsts->isProcNull(target))
        return GTI_ANALYSIS_SUCCESS;

    const void* returnaddr = myLIdMod->getInfoForId(pId, lId).codeptr;

    // translate target rank to world comm
    int realTarget = translateRank(myWinMod->getWin(pId, win)->getComm(), target);

    int levelId;
    getLevelIdForApplicationRank(realTarget, &levelId);
    int origin = myPIdMod->getInfoForId(pId).rank;

    // Note: We have two RMA operations here: MPI_Get and MPI_Accumulate, both are atomic on the basic
    // elements. For simplicity in handling, we split them in two separate RMA operations.
 
    // Accumulate call
    // get a unique Id for the call such that we can match origin and target memory operations
    MustRMAId callId = nextId();
    addOriginOp(callId, pId, lId, false, originaddr, origincount, origintype, realTarget, win, requesttype, ann, (void*) returnaddr, functionaddr);
    passTargetRMAOpAcross(origin, origin, callId, pId, lId, true, true,
            realTarget, targetdisp, origincount, origintype, targettype, win, (void*) returnaddr, functionaddr, levelId);

    // Get call
    callId = nextId();
    addOriginOp(callId, pId, lId, true, resultaddr, resultcount, resulttype, realTarget, win, requesttype, ann, (void*) returnaddr, functionaddr);
    passTargetRMAOpAcross(origin, origin, callId, pId, lId, false, true,
            realTarget, targetdisp, targetcount, origintype, targettype, win, (void*) returnaddr, functionaddr, levelId);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winLock
//=============================
GTI_ANALYSIS_RETURN RMATrack::winCreate(
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {

    myWinLockAddrs[win] = ann;
    myWinCreateAddrs[win] = ann;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winLock
//=============================
GTI_ANALYSIS_RETURN RMATrack::winLock(
        MustParallelId pId,
        MustLocationId lId,
        int lock_type,
        int rank,
        MustWinType win,
        void* ann) {
    int realRank = translateRank(myWinMod->getWin(pId, win)->getComm(), rank);

    if (lock_type == MPI_LOCK_EXCLUSIVE) {
        myExclusiveLockSet.insert(std::make_pair(win, realRank));

        // if this is a local win lock, store TSan annotation address
        if (realRank == myPIdMod->getInfoForId(pId).rank) {
            std::cout << "insert (win " << win << "): " << ann << std::endl;
            myWinLockAddrs.insert(std::make_pair(win, ann));
        }
    }

    myWinAccessMode[win] = WINDOW_ACCESS_PASSIVE;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winUnlock
//=============================
GTI_ANALYSIS_RETURN RMATrack::winUnlock(
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win) {
    int realRank = translateRank(myWinMod->getWin(pId, win)->getComm(), rank);
    myExclusiveLockSet.erase(std::make_pair(win, realRank));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// originCompletionAll
//=============================
GTI_ANALYSIS_RETURN RMATrack::originCompletionAll (
                MustParallelId pId,
                MustLocationId lId,
                MustWinType win,
                void* ann
                ) {
    std::list<MustRMAId> completedOps = myOrigRMAMap.getWinOps(win); 

    // notify listeners
    notifyOriginOpComplete(pId, lId, completedOps);

    myOrigRMAMap.removeOps(completedOps);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// originCompletionRank
//=============================
GTI_ANALYSIS_RETURN RMATrack::originCompletionRank (
                MustParallelId pId,
                MustLocationId lId,
                int targetRank,
                MustWinType win,
                void* ann
                ) {
    // get all origin memory operations with the corresponding target rank,
    // these are completed
    std::list<MustRMAId> completedOps = myOrigRMAMap.getWinTargetOps(win, targetRank); 

    // notify listeners
    notifyOriginOpComplete(pId, lId, completedOps);

    myOrigRMAMap.removeOps(completedOps);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// incrementRMAEpoch
//=============================
void RMATrack::incrementRMAEpoch (
                MustParallelId pId,
                MustWinType win
                ) {
    auto it = myRMAActiveEpochCounter.find(win);
    if (it != myRMAActiveEpochCounter.end()) {
        // increment counter for *all* processes
        std::transform(it->second.begin(), it->second.end(), it->second.begin(),
            [](int x) { return x + 1; });
    } else {
        std::vector<int>epochCounter(myWinMod->getWin(pId, win)->getComm()->getGroup()->getSize(), 0);
        myRMAActiveEpochCounter.insert(std::make_pair(win, epochCounter));
    }
}

//=============================
// incrementRMAEpochRank
//=============================
void RMATrack::incrementRMAEpochRank (
                MustParallelId pId,
                MustWinType win,
                int rank
                ) {
    auto it = myRMAActiveEpochCounter.find(win);
    if (it == myRMAActiveEpochCounter.end()) {
        std::vector<int>epochCounter(myWinMod->getWin(pId, win)->getComm()->getGroup()->getSize(), 0);
        myRMAActiveEpochCounter.insert(std::make_pair(win, epochCounter));
    }

    myRMAActiveEpochCounter[win][rank]++;
}


//=============================
// activeTargetCompletionAll
//=============================
GTI_ANALYSIS_RETURN RMATrack::activeTargetCompletionAll (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {
    myWinAccessMode[win] = WINDOW_ACCESS_ACTIVE;

    // Active target completion implies memory consistency, happens-before synchronization
    // and window synchronization. Thus, we know that all items currently in the state map
    // in state INIT can be directly annotated without waiting for any further action.
    std::map<int, std::vector<std::list<MustRMAId> > >::iterator it;
    for (it = myTgtStateMap[win].begin(); it != myTgtStateMap[win].end(); ++it) {
        // get all items in state INIT
        std::list<MustRMAId>& initItems = it->second[TARGET_OP_STATE_INIT];

        // We have to take into account that we could have already received RMA ops of
        // the "next" RMA access epoch. Only consider RMA ops that have an access epoch smaller
        // than the current access epoch.
        std::list<MustRMAId> completedItems;
        for (auto it = initItems.begin(); it != initItems.end(); ) {
#ifdef MUST_DEBUG
            std::cout << "[RMATrack] epoch "<< myTgtRMAMap.getOp(*it)->getRMAEpoch() << " < " << myRMAActiveEpochCounter[win][myPIdMod->getInfoForId(pId).rank] << "?" << std::endl;
#endif
            if (myTgtRMAMap.getOp(*it)->getRMAEpoch() <= myRMAActiveEpochCounter[win][myPIdMod->getInfoForId(pId).rank]) {
                completedItems.push_back(*it);
                // remove the RMA ids from the state map
                it = initItems.erase(it);
            } else {
                it++;
            }
        }

        notifyTargetOpComplete(pId, lId, completedItems);

        // remove the RMA call information
        myTgtRMAMap.removeOps(completedItems);
    }

    // start next access epoch
    incrementRMAEpoch(pId, win);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winStart
//=============================
GTI_ANALYSIS_RETURN RMATrack::winStart (
        MustParallelId pId,
        MustLocationId lId,
        MustGroupType group,
        MustWinType win,
        void* ann) {
    myWinAccessMode[win] = WINDOW_ACCESS_ACTIVE;

    myWinCompleteGroupMap.insert(std::make_pair(win, group));

    for (auto const& rank : myGrpMod->getGroup(pId, group)->getGroup()->getMapping()) {
        incrementRMAEpochRank(pId, win, rank);
    }
    
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winComplete
//=============================
GTI_ANALYSIS_RETURN RMATrack::winComplete (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {

    MustGroupType group = myWinCompleteGroupMap[win];
    I_GroupTable* groupTable = myGrpMod->getGroup(pId, group)->getGroup();
    myWinCompleteGroupMap.erase(win);

    int origin = myPIdMod->getInfoForId(pId).rank;

    for (auto const& rank : groupTable->getMapping()) {
        int epoch = myRMAActiveEpochCounter[win][rank];
#ifdef MUST_DEBUG
        std::cout << "passTargetCompletionAcross("<<rank<<", "<<pId<<", "<<lId<<", "<<origin<<", "<<win<<", "<<epoch<<")"<<std::endl;
#endif
        passTargetCompletionAcross(
            rank,
            pId,
            lId,
            origin,
            win,
            epoch);
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// winPost
//=============================
GTI_ANALYSIS_RETURN RMATrack::winPost (
        MustParallelId pId,
        MustLocationId lId,
        MustGroupType group,
        MustWinType win,
        void* ann) {
    myWinAccessMode[win] = WINDOW_ACCESS_ACTIVE;

    myWinWaitGroupMap.insert(std::make_pair(win, group));
    
    for (auto const& rank : myGrpMod->getGroup(pId, group)->getGroup()->getMapping()) {
        incrementRMAEpochRank(pId, win, rank);
    }

    myRMAActiveEpochAddr[std::make_pair(win, myRMAActiveEpochCounter[win][myPIdMod->getInfoForId(pId).rank])] = ann;
    

    return GTI_ANALYSIS_SUCCESS;
}


//=============================
// winWait
//=============================
GTI_ANALYSIS_RETURN RMATrack::winWait (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {

    MustGroupType group = myWinWaitGroupMap[win];
    I_GroupTable* groupTable = myGrpMod->getGroup(pId, group)->getGroup();
    myWinWaitGroupMap.erase(win);
    int ownRank = myPIdMod->getInfoForId(pId).rank;

    // Wait for completion events from all ranks in the group (then we can be sure that we received all RMA calls)
    for (auto const& rank : groupTable->getMapping()) {
        if (rank == ownRank)
            continue; // do not have to check for own rank

        // Wait until we received the completion event with the corresponding epoch
        while (myRMARemoteActiveEpochCounter.find(win) == myRMARemoteActiveEpochCounter.end() ||
            myRMARemoteActiveEpochCounter[win][rank] < myRMAActiveEpochCounter[win][rank]) {
#ifdef MUST_DEBUG
                if (myRMARemoteActiveEpochCounter.find(win) == myRMARemoteActiveEpochCounter.end()) {
                    std::cout << "[RMATrack] Epoch counter does not exist" << std::endl;
                } else {
                    std::cout << "[RMATrack] WIN WAIT: epoch " 
                              << myRMARemoteActiveEpochCounter[win][rank] 
                              << "< "
                              << myRMAActiveEpochCounter[win][rank] 
                              << "?" << std::endl;
                }
#endif
                myPlaceMod->testIntralayer();
            }
    }

    // Active target completion implies memory consistency, happens-before synchronization
    // and window synchronization. Thus, we know that all items currently in the state map
    // in state INIT can be directly annotated without waiting for any further action.
    std::map<int, std::vector<std::list<MustRMAId> > >::iterator it;
    for (it = myTgtStateMap[win].begin(); it != myTgtStateMap[win].end(); ++it) {
        // get all items in state INIT
        std::list<MustRMAId>& initItems = it->second[TARGET_OP_STATE_INIT];
        // We have to take into account that we could have already received RMA ops of
        // the "next" RMA access epoch. Only consider RMA ops that have an access epoch smaller
        // than the current access epoch.
        std::list<MustRMAId> completedItems;
        for (auto it = initItems.begin(); it != initItems.end(); ) {
            I_TargetRMAOp* rmaOp = myTgtRMAMap.getOp(*it);
#ifdef MUST_DEBUG
            std::cout << "[RMATrack] epoch "<< rmaOp->getRMAEpoch() << " < " << myRMAActiveEpochCounter[win][rmaOp->getOrigin()] << "?" << std::endl;
#endif
            if (rmaOp->getRMAEpoch() <= myRMAActiveEpochCounter[win][rmaOp->getOrigin()]
                && groupTable->containsWorldRank(rmaOp->getOrigin(), NULL)) {
                completedItems.push_back(*it);

                void* annAddr = myRMAActiveEpochAddr[std::make_pair(win, myRMAActiveEpochCounter[win][myPIdMod->getInfoForId(pId).rank])];
                rmaOp->setFiber(annAddr);
                // remove the RMA ids from the state map
                it = initItems.erase(it);
            } else {
                it++;
            }
        }

        notifyTargetOpComplete(pId, lId, completedItems);

        // remove the RMA call information
        myTgtRMAMap.removeOps(completedItems);
    }



    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// privateWindowUpdate
//=============================
GTI_ANALYSIS_RETURN RMATrack::privateWindowUpdate (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {

    // Private window update completes all synchronized RMA operations (state = CONSISTENT_HB)
    // of the separate memory model.
    std::map<int, std::vector<std::list<MustRMAId> > >::iterator it;
    for (it = myTgtStateMap[win].begin(); it != myTgtStateMap[win].end(); ++it) {
        // get all items in state CONSISTENT_HB, this can be only RMA operations
        // of the separate memory model
        std::list<MustRMAId>& consistentHbItems = it->second[TARGET_OP_STATE_CONSISTENT_HB];

        notifyTargetOpComplete(pId, lId, consistentHbItems);

        // remove the RMA call information
        myTgtRMAMap.removeOps(consistentHbItems);
        // remove the RMA ids from the state map
        consistentHbItems.clear();
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// passiveTargetCompletionAll
//=============================
GTI_ANALYSIS_RETURN RMATrack::passiveTargetCompletionAll (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {

    int origin = myPIdMod->getInfoForId(pId).rank;

    // Get ranks (translated to world rank) associated with this communicator
    const std::vector<int>& groupMapping = myWinMod->getWin(pId, win)->getComm()->getGroup()->getMapping();

    for (std::vector<int>::const_iterator it = groupMapping.begin(); it != groupMapping.end(); ++it) {
#ifdef MUST_DEBUG
    std::cout << "[RMATrack] passTargetCompletionAcross("<<*it<<", "<<pId<<", "<<lId<<", "<<origin<<", "<<win<<", -1)"<<std::endl;
#endif
        passTargetCompletionAcross(
                *it,
                pId,
                lId,
                origin,
                win,
                -1); // passive target completion does not need epoch counter, we have to find the matching call with happened-before analysis
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// passiveTargetCompletionRank
//=============================
GTI_ANALYSIS_RETURN RMATrack::passiveTargetCompletionRank (
                MustParallelId pId,
                MustLocationId lId,
                int targetRank,
                MustWinType win,
                void* ann
                ) {
    // translate target rank to world comm
    int realTarget = translateRank(myWinMod->getWin(pId, win)->getComm(), targetRank);
#ifdef MUST_DEBUG
    std::cout << "[RMATrack] passTargetCompletionAcross("<<realTarget<<", "<<pId<<", "<<lId<<", "<<myPIdMod->getInfoForId(pId).rank<<", "<<win<<", -1)"<<std::endl;
#endif
    passTargetCompletionAcross(
            realTarget,
            pId,
            lId,
            myPIdMod->getInfoForId(pId).rank, // origin rank
            win,
            -1); // passive target completion does not need epoch counter, we have to find the matching call with happened-before analysis

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// completedRequest
//=============================
GTI_ANALYSIS_RETURN RMATrack::completedRequest (
		MustParallelId pId,
		MustLocationId lId,
		MustRequestType request)
{
    // Are there matching RMA ops?
    std::list<MustRMAId> completedOps = myOrigRMAMap.getReqOps(request);
    if (!completedOps.size()) return GTI_ANALYSIS_SUCCESS;

    // corresponding RMA operation are finished - notify listeners
    notifyOriginOpComplete(pId, lId, completedOps);

    myOrigRMAMap.removeOps(completedOps);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// completedRequests
//=============================
GTI_ANALYSIS_RETURN RMATrack::completedRequests (
		MustParallelId pId,
		MustLocationId lId,
		MustRequestType *requests,
		int count)
{
    for (int i = 0; i < count; i++)
    {
        GTI_ANALYSIS_RETURN ret = completedRequest(pId, lId, requests[i]);
        if (ret != GTI_ANALYSIS_SUCCESS)
            return ret;
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// getOriginRMAOp 
//=============================
I_OriginRMAOp* RMATrack::getOriginRMAOp(MustRMAId callId) {
    return myOrigRMAMap.getOp(callId);
}

//=============================
// getPersistentOriginRMAOp 
//=============================
I_OriginRMAOpPersistent* RMATrack::getPersistentOriginRMAOp(MustRMAId callId) {
    OriginRMAOp* op = myOrigRMAMap.getOp(callId);
    if (op)
        op->incRefCount();

    return op;
}

//=============================
// getTargetRMAOp
//=============================
I_TargetRMAOp* RMATrack::getTargetRMAOp(MustRMAId callId) {
    return myTgtRMAMap.getOp(callId);
}

//=============================
// getPersistentTargetRMAOp
//=============================
I_TargetRMAOpPersistent* RMATrack::getPersistentTargetRMAOp(MustRMAId callId) {
    TargetRMAOp* op = myTgtRMAMap.getOp(callId);
    if (op)
        op->incRefCount();

    return op;
}

//=============================
// getWinLockAddr
//=============================
void* RMATrack::getWinLockAddr(
        MustWinType win) {
    return myWinLockAddrs[win];
}


//=============================
// Destructor.
//=============================
RMATrack::~RMATrack (void)
{
#ifdef MUST_DEBUG
    if (myTgtRMAMap.size() > 0) {
        std::cout << "[RMATrack] Error: Lost RMA calls that were not annotated: " << myTgtRMAMap.size() << std::endl;
    }
#endif
    if (myPIdMod)
        destroySubModuleInstance ((I_Module*) myPIdMod);
    myPIdMod = NULL;

    if (myLogger)
        destroySubModuleInstance ((I_Module*) myLogger);
    myLogger = NULL;

    if (myConsts)
        destroySubModuleInstance ((I_Module*) myConsts);
    myConsts = NULL;

    if (myDatMod)
        destroySubModuleInstance ((I_Module*) myDatMod);
    myDatMod = NULL;

    if (myReqMod)
        destroySubModuleInstance ((I_Module*) myReqMod);
    myReqMod = NULL;

    if (myGrpMod)
        destroySubModuleInstance ((I_Module*) myGrpMod);
    myGrpMod = NULL;

    if (myWinMod)
        destroySubModuleInstance ((I_Module*) myWinMod);
    myWinMod = NULL;

    if (myTSanMod)
        destroySubModuleInstance ((I_Module*) myTSanMod);
    myTSanMod = NULL;
}

//=============================
// nextId
//=============================
MustRMAId RMATrack::nextId() {
    return ++myRMAId;
}

//=============================
// translateRank
//=============================
int RMATrack::translateRank (I_Comm* comm, int rank)
{
    int ret;
    if (rank != myConsts->getAnySource()) {
        if (!comm->isIntercomm())
            comm->getGroup()->translate(rank, &ret);
        else
            comm->getRemoteGroup()->translate(rank, &ret);
    } else {
        ret = rank;
    }

    return ret;
}

//=============================
// calcIntervalList 
//=============================
MustMemIntervalListType RMATrack::calcIntervalList(
        I_Datatype * typeinfo,
        MustAddressType buffer,
        int count)
{
    MustMemIntervalListType ret;
    BlockInfo& blockInfo = typeinfo->getBlockInfo();
    MustAddressType extent = typeinfo->getExtent();
    MustAddressType size = typeinfo->getSize();
    ret = buildMemIntervallist(blockInfo, extent, size, buffer, (MustRequestType) 0, true, typeinfo, count, buffer);

    return ret;
}

//=============================
// addOriginOp
//=============================
void RMATrack::addOriginOp(
                    MustRMAId callId,
                    MustParallelId pId,
                    MustLocationId lId,
                    bool isStore,
                    MustAddressType addr,
                    int count,
                    MustDatatypeType dataType,
                    int target,
                    MustWinType win,
                    MustRequestType request,
                    void* annAddr,
                    void* returnaddr,
                    void* functionaddr) {
    I_Datatype* dataTypeHandle = myDatMod->getDatatype(pId, dataType);
    // we need the communicator of the window for the rank translation
    I_Comm* commHandle = myWinMod->getWin(pId, win)->getComm();

    // calculate the accessed memory regions
    MustMemIntervalListType intervalList = calcIntervalList(dataTypeHandle, addr, count);

    OriginRMAOp* memOp = new OriginRMAOp();
    memOp->myPId = pId;
    memOp->myLId = lId;
    memOp->myOrigin = myPIdMod->getInfoForId(pId).rank,
    memOp->myTarget = target,
    memOp->myIsStore = isStore,
    memOp->myMemIntervals = intervalList,
    memOp->myWin = myWinMod->getPersistentWin(pId, win);
    memOp->myWinId = win;
    // TODO: getPersistentRequest will return NULL, the request information is added too late
    // to the RequestTrack module on the tool thread place. Currently, we work only with the requestId.
    memOp->myRequest = myReqMod->getPersistentRequest(pId, request);
    memOp->myRequestId = request;
    memOp->myFiber = annAddr;
    memOp->myReturnAddr = returnaddr;
    memOp->myFunctionAddr = functionaddr;

    // add new access to pending operations
    myOrigRMAMap.addOp(callId, memOp);

    notifyOriginOpStart(callId);
}

//=============================
// addTargetOp
//=============================
void RMATrack::addTargetOp (
                int origin,
                MustRMAId rmaId,
                MustParallelId pId,
                MustLocationId lId,
                bool isStore,
                bool isAtomic,
                bool isLocked,
                int target,
                int disp,
                int count,
                I_Datatype* originDatatype,
                I_DatatypePersistent* targetDatatype,
                MustWinType winId,
                int epoch,
                int accessMode,
                void* ann,
                void* returnAddr,
                void* functionAddr) {
    MustMemIntervalListType intervalList;
    I_WinPersistent* win = myWinMod->getPersistentWin(target, winId);

    // calculate start address
    MustAddressType startAddr = win->getBase() + disp * win->getDispUnit();

    // We have to differentiate between load and store for memory interval calulcation.
    // In case of a load, the accessed memory interval is calculated based on targetCount
    // and targetDatatype.
    // In case of a store, the accessed memory interval is calculated based on originCount
    // and originDatatype.
    if (!isStore && targetDatatype) {
        intervalList = calcIntervalList(targetDatatype, startAddr, count);
    } else if (isStore && originDatatype) {
        intervalList = calcIntervalList(originDatatype, startAddr, count);
    } else {
        // something went wrong
        assert(0);
    }

    TargetRMAOp* memOp = new TargetRMAOp();
    memOp->myPId = pId;
    memOp->myLId = lId;
    memOp->myOrigin = origin;
    memOp->myTarget = target;
    memOp->myIsStore = isStore;
    memOp->myIsAtomic = isAtomic;
    memOp->myIsLocked = isLocked;
    memOp->myMemIntervals = intervalList;
    memOp->myWin = win;
    memOp->myWinId = winId;
    memOp->myRMAEpoch = epoch;
    memOp->myWinAccessMode = (MustWinAccessMode) accessMode;
    memOp->myTargetDatatype = targetDatatype;
    memOp->myFiber = ann;
    memOp->myReturnAddr = returnAddr;
    memOp->myFunctionAddr = functionAddr;

    // add new access to pending operations
    myTgtRMAMap.addOp(rmaId, memOp);

    notifyTargetOpStart(rmaId);

    // check whether myTgtStateMap[winId][origin] exists
    if (myTgtStateMap[winId].find(origin) == myTgtStateMap[winId].end()) {
        // initialize vector with states
        myTgtStateMap[winId].insert(
                std::make_pair(origin,
                               std::vector<std::list<MustRMAId> >(TARGET_OP_STATE_COUNT, std::list<MustRMAId>())));
    }

    // Special case: Check for conflict to operations from the same origin rank
    // that are not already consistent.
    // If the origin locks a target window and performs two conflicting RMA operations
    // on it, then it will not be detected through the TSan annotations, because they are
    // annotated as lock-protected.
    std::list<MustRMAId>& initItems = myTgtStateMap[winId][origin][TARGET_OP_STATE_INIT];
    for (auto& rmaId: initItems) {
        I_TargetRMAOp* op = getTargetRMAOp(rmaId);
        const MustMemIntervalListType& memIntervals = op->getMemIntervals();

        // If both accesses are reads, then we can skip
        if (!isStore && !op->isStore())
            continue;

        // If both accesses are accumulates *and* have the same data type, we can skip the pairs, they must be save
        if (isAtomic && op->isAtomic())
        {
            MustAddressType errorpos;
            targetDatatype->isEqualB(1, op->getTargetDatatype(), 1, &errorpos);
            if (errorpos == 0) {
                // TODO: Also have to check alignment
                continue;
            }
        }
        
        // Iterate over all memory intervals from *this* RMA call
        for (MustMemIntervalListType::iterator it1 = intervalList.begin(); it1 != intervalList.end(); ++it1) {
            for (int i1 = 0; i1 < it1->count; ++i1) {
                MustAddressType start1 = it1->baseAddress + (it1->stride * i1);
                MustAddressType end1 = start1 + it1->blocksize;

                // Compare with all memory intervals belonging the RMA call "rmaId"
                for (MustMemIntervalListType::iterator it2 = memIntervals.begin(); it2 != memIntervals.end(); ++it2) {
                    for (int i2 = 0; i2 < it2->count; ++i2) {
                        MustAddressType start2 = it2->baseAddress + (it2->stride * i2);
                        MustAddressType end2 = start2 + it2->blocksize;

                        // Report race if intervals are overlapping
                        if ((start1 >= start2 && start1 <= end2) || (start2 >= start1 && start2 <= end1)) {
                            std::list <std::pair <MustParallelId, MustLocationId> > refs;
                            refs.push_back(std::make_pair(pId, lId));
                            refs.push_back(std::make_pair(op->getPId(), op->getLId()));
                            std::stringstream stream;
                            stream <<  "Found RMA data race (overlapping accesses) on rank " << target << " originating from rank " << origin;
                            myLogger->createMessage(
                                MUST_ERROR_REQUEST_NOT_KNOWN,
                                pId,
                                lId,
                                MustErrorMessage,
                                stream.str(),
                                refs
                            );
                        }

                    }
                }
            }
        }
    }

    myTgtStateMap[winId][origin][TARGET_OP_STATE_INIT].push_back(rmaId);
}

//=============================
// passTargetRMAOpAcross
//=============================
void RMATrack::passTargetRMAOpAcross (
                int rank,
                int origin,
                MustRMAId rmaId,
                MustParallelId pId,
                MustLocationId lId,
                bool isStore,
                bool isAtomic,
                int target,
                int disp,
                int count,
                MustDatatypeType originDatatype,
                MustDatatypeType targetDatatype,
                MustWinType win,
                void* returnAddr,
                void* functionAddr,
                int toPlaceId) {
    if (!myPassTargetRMAOpAcrossFunc)
        return;

    I_DatatypePersistent* originDatatypeInfo = myDatMod->getPersistentDatatype(origin, originDatatype);
    I_DatatypePersistent* targetDatatypeInfo = myDatMod->getPersistentDatatype(origin, targetDatatype);
    I_Win* winInfo = myWinMod->getPersistentWin(origin, win);

    const bool isLocked = myExclusiveLockSet.find(std::make_pair(win, target)) != myExclusiveLockSet.end();

    // get RMA epoch (only important for active target)
    auto it = myRMAActiveEpochCounter.find(win);
    int epoch = -1;
    if (it != myRMAActiveEpochCounter.end())
        epoch = it->second[target];

    if (origin == target) {
        // local call, directly add target operation
        addTargetOp(
                origin,
                rmaId,
                pId,
                lId,
                isStore,
                isAtomic,
                isLocked,
                target,
                disp,
                count,
                originDatatypeInfo,
                targetDatatypeInfo,
                win,
                epoch,
                myWinAccessMode[win],
                myOrigRMAMap.getOp(rmaId)->getFiber(),
                returnAddr,
                functionAddr);
    } else {
        // remote call, transmit to target process
        MustRemoteIdType originDatatypeRemoteId = 0,
                         targetDatatypeRemoteId = 0,
                         winRemoteId = 0;

        // pass location of call across
        myLIdMod->passLocationToPlace(pId, lId, toPlaceId);

        if (originDatatypeInfo)
            myDatMod->passDatatypeAcross(rank, originDatatypeInfo, toPlaceId, &originDatatypeRemoteId);

        if (targetDatatypeInfo)
            myDatMod->passDatatypeAcross(rank, targetDatatypeInfo, toPlaceId, &targetDatatypeRemoteId);

        if (winInfo)
            myWinMod->passWinAcross(rank, winInfo, toPlaceId, &winRemoteId);


        if (origin != target)
            // only non-local calls count as pending RMA calls
            myMPIVectorClock->addPendingRMACall(target);

#ifdef MUST_DEBUG
        std::stringstream msg;
        msg << "[RMATrack] Pass across"
                  << "origin=" << origin
                  << ",rmaId=" << rmaId
                  << ",pId=" << pId
                  << ",lId=" << lId
                  << ",isStore=" << isStore
                  << ",isAtomic=" << isAtomic
                  << ",isLocked=" << isLocked
                  << ",target=" << target
                  << ",disp=" << disp
                  << ",count=" << count
                  << ",originDatatypeRemoteId=" << originDatatypeRemoteId
                  << ",targetDatatypeRemoteId=" << targetDatatypeRemoteId
                  << ",winRemoteId=" << winRemoteId
                  << ",epoch=" << epoch
                  << ",accessMode=" << myWinAccessMode[win]
                  << ",clockvalue=" << myVectorClock->getClockValue(target)
                  << ",returnAddr=" << returnAddr
                  << ",functionAddr=" << functionAddr
                  << ",toPlaceId=" << toPlaceId
                  << std::endl;
        std::cout << msg.str();
#endif

        // pass the actual RMA op across
        (*myPassTargetRMAOpAcrossFunc) (
                origin,
                rmaId,
                pId,
                lId,
                isStore,
                isAtomic,
                isLocked,
                target,
                disp,
                count,
                originDatatypeRemoteId,
                targetDatatypeRemoteId,
                winRemoteId,
                epoch,
                myWinAccessMode[win],
                myVectorClock->getClockValue(target),//myMPIVectorClock->get()[target],
                returnAddr,
                functionAddr,
                toPlaceId
                );
    }
}

//=============================
// passTargetCompletionAcross
//=============================
void RMATrack::passTargetCompletionAcross (
        int rank,
        MustParallelId pId,
        MustLocationId lId,
        int origin,
        MustWinType win,
        int epoch) {

    if (!myPassTargetCompletionAcrossFunc)
        return;

    int toPlaceId;
    getLevelIdForApplicationRank(rank, &toPlaceId);

    if (rank == origin) {
        // send target completion to own rank
        // no passing across of pIds, lIds or windows needed
        (*myPassTargetCompletionAcrossFunc) (
            pId,
            lId,
            origin,
            rank,
            win,
            epoch,
            toPlaceId
        );
    } else {
        // pass location of call across
        myLIdMod->passLocationToPlace(pId, lId, toPlaceId);

        I_Win* winInfo = myWinMod->getPersistentWin(origin, win);
        MustRemoteIdType winRemoteId = 0;

        if (winInfo)
            myWinMod->passWinAcross(origin, winInfo, toPlaceId, &winRemoteId);
        else
            assert(0);

        myMPIVectorClock->addPendingRMACall(rank);

        (*myPassTargetCompletionAcrossFunc) (
            pId,
            lId,
            origin,
            rank,
            winRemoteId,
            epoch,
            toPlaceId
        );
    }
}



//=============================
// notifyOriginOpStart
//=============================
void RMATrack::notifyOriginOpStart(MustRMAId op) {
    if (myNotifyOriginOpStartFunc)
        (*myNotifyOriginOpStartFunc) (op);
}

//=============================
// notifyOriginOpComplete
//=============================
void RMATrack::notifyOriginOpComplete(
        MustParallelId pId,
        MustLocationId lId,
        const std::list<MustRMAId>& completedOps) {
    MustRMAId* rmaIds = new MustRMAId[completedOps.size()];
    std::copy(completedOps.begin(), completedOps.end(), rmaIds);
    if (myNotifyOriginOpCompleteFunc)
        (*myNotifyOriginOpCompleteFunc) (pId, lId, rmaIds, completedOps.size());
}

//=============================
// notifyTargetOpStart
//=============================
void RMATrack::notifyTargetOpStart(MustRMAId op) {
    if (myNotifyTargetOpStartFunc)
        (*myNotifyTargetOpStartFunc) (op);
}

//=============================
// notifyTargetOpComplete
//=============================
void RMATrack::notifyTargetOpComplete(
        MustParallelId pId,
        MustLocationId lId,
        const std::list<MustRMAId>& completedOps) {
    MustRMAId* rmaIds = new MustRMAId[completedOps.size()];
    std::copy(completedOps.begin(), completedOps.end(), rmaIds);
    if (myNotifyTargetOpCompleteFunc)
        (*myNotifyTargetOpCompleteFunc) (pId, lId, rmaIds, completedOps.size());
}
