/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OriginChecks.cpp
 *       @see must::OriginChecks.
 *
 *  @date 13.06.2017
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"
#include "OriginChecks.h"
#include "MustEnums.h"
#include "MustDefines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sstream>
#include <fstream>

using namespace must;

mGET_INSTANCE_FUNCTION(OriginChecks)
mFREE_INSTANCE_FUNCTION(OriginChecks)
mPNMPI_REGISTRATIONPOINT_FUNCTION(OriginChecks)

//=============================
// Constructor.
//=============================
OriginChecks::OriginChecks (const char* instanceName)
    : ModuleBase<OriginChecks, I_OriginChecks> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUBMODULES 9
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
    myLIdMod = (I_LocationAnalysis*) subModInstances[5];
    myWinMod = (I_WinTrack*) subModInstances[6];
    myTSanMod = (I_TSan*) subModInstances[7];
    myRMAMod = (I_RMATrack*) subModInstances[8];

    //Initialize module data

    // Create fiber pool
    myTSanMod->annotateIgnoreSyncBegin();
    for (int i = 0; i < FIBER_POOL_SIZE; ++i) {
        myFiberPool.push_back(myTSanMod->createFiber(0));
    }
    myTSanMod->annotateIgnoreSyncEnd();

    int fiberPoolCounter = 0;
}

/**
 * @see I_OriginChecks::originOpStart
 */
GTI_ANALYSIS_RETURN OriginChecks::originOpStart(
        MustRMAId rmaId) {
    I_OriginRMAOpPersistent* op = myRMAMod->getPersistentOriginRMAOp(rmaId);

#ifdef MUST_DEBUG
    std::stringstream msg;
    msg << "OriginRMAOp STARTED:";
    msg << "callId: " << rmaId;
    msg << ", pId: " << op->getPId();
    msg << ", lId: " << op->getLId();
    msg << ", isStore: " << op->isStore();
    msg << ", win: " << op->getWin();
    msg << ", request: " << op->getRequest();
    msg << ", target: " << op->getTarget();
    msg << ", annotation address: " << op->getFiber();
    msg << ", return addr: " << op->getReturnAddr();
    msg << ", function addr: " << op->getFunctionAddr();
    msg << std::endl;
    std::cout << msg.str();
#endif

    return GTI_ANALYSIS_SUCCESS;
}

/**
 * @see I_OriginChecks::originOpComplete
 */
GTI_ANALYSIS_RETURN OriginChecks::originOpComplete(
        MustParallelId pId,
        MustLocationId lId,
        MustRMAId* rmaId,
        int rmaIdLen) {
#ifdef MUST_DEBUG
    std::cout << "=== ORIGIN OPS COMPLETED ===" << std::endl;
#endif

    // get current fiber to switch back at the end
    void* curFiber = myTSanMod->getCurrentFiber();

    for (int i = 0; i < rmaIdLen; ++i) {
        I_OriginRMAOp* op = myRMAMod->getOriginRMAOp(rmaId[i]);

#ifdef MUST_DEBUG
        std::stringstream msg;
        msg << "OriginRMAOp COMPLETED: ";
        msg << "callId: " << rmaId[i];
        msg << ", pId: " << op->getPId();
        msg << ", lId: " << op->getLId();
        msg << ", isStore: " << op->isStore();
        msg << ", win: " << op->getWin();
        msg << ", request: " << op->getRequest();
        msg << ", target: " << op->getTarget();
        msg << ", annotation address: " << op->getFiber();
        msg << ", return addr: " << op->getReturnAddr();
        msg << ", function addr: " << op->getFunctionAddr();
        msg << std::endl;
        std::cout << msg.str();
#endif

        // select fiber from fiber pool (depending on origin rank)
        // TODO: reusing fibers might lead to false negatives.
        void* tempFiber = myFiberPool.at(fiberPoolCounter++ % FIBER_POOL_SIZE); 

        // Switch to fiber of operation without synchronization (flag: 1)
        myTSanMod->switchToFiber(tempFiber, 1);
        // Load corresponding vector clock
        myTSanMod->annotateHappensAfter(pId, lId, op->getFiber());
        annotateMemAccess(op->getPId(), op->getLId(), op->getMemIntervals(), op->isStore(), op->getReturnAddr(), op->getFunctionAddr());

        // Switch to current thread *with* synchronization
        // (memory accesses afterwards do *not* conflict with *this* RMA call)
        // Note: This has to be done for every annotation of an RMA origin operation.
        void* myaddr = &pId;
        myTSanMod->annotateHappensBefore(pId, lId, myaddr);
        myTSanMod->switchToFiber(curFiber, 0);
        myTSanMod->annotateHappensAfter(pId, lId, myaddr);
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// annotateMemAccess
//=============================
void OriginChecks::annotateMemAccess(
        MustParallelId pId,
        MustLocationId lId,
        const MustMemIntervalListType& memIntervals,
        bool isStore,
        void* returnAddr,
        void* funcAddr) {
    myTSanMod->annotateFuncEntry(pId, lId);
    for (MustMemIntervalListType::iterator it = memIntervals.begin(); it != memIntervals.end(); ++it) {
#ifdef MUST_DEBUG
        std::stringstream msg;
        msg << "stride: " << it->stride;
        msg << ", blocksize: " << it->blocksize;
        msg << ", count: " << it->count;
        msg << ", repetition: " << it->repetition;
        msg << std::endl;
        std::cout << msg.str();
#endif

        for (int i = 0; i < it->count; ++i) {
            MustAddressType start = it->baseAddress + (it->stride * i);
#ifdef MUST_DEBUG
            std::stringstream msg;
            msg << "[" << start << "," << start + it->blocksize << "]" << std::endl;
            msg << "[" << (void*) start << "," << (void*) (start + it->blocksize) << "]" << std::endl;
            std::cout << msg.str();
#endif
            // Increment function address by one here, because TSan expects a return address and decrements
            // the pointer by one.
            if (isStore)
                myTSanMod->annotateMemoryWritePC(pId, lId, start, it->blocksize, static_cast<char*>(funcAddr) + 1);
            else
                myTSanMod->annotateMemoryReadPC(pId, lId, start, it->blocksize, static_cast<char*>(funcAddr) + 1);
        }
    }
    myTSanMod->annotateFuncExit();
}


//=============================
// Destructor.
//=============================
OriginChecks::~OriginChecks (void)
{
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

    if (myWinMod)
        destroySubModuleInstance ((I_Module*) myWinMod);
    myWinMod = NULL;

    if (myTSanMod)
        destroySubModuleInstance ((I_Module*) myTSanMod);
    myTSanMod = NULL;

    if (myRMAMod)
        destroySubModuleInstance ((I_Module*) myRMAMod);
    myRMAMod = NULL;
}
