/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TargetChecks.cpp
 *       @see must::TargetChecks.
 *
 *  @date 13.06.2017
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"
#include "TargetChecks.h"
#include "MustEnums.h"
#include "MustDefines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sstream>
#include <fstream>

using namespace must;

mGET_INSTANCE_FUNCTION(TargetChecks)
mFREE_INSTANCE_FUNCTION(TargetChecks)
mPNMPI_REGISTRATIONPOINT_FUNCTION(TargetChecks)

//=============================
// Constructor.
//=============================
TargetChecks::TargetChecks (const char* instanceName)
    : ModuleBase<TargetChecks, I_TargetChecks> (instanceName)
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
}

/**
 * @see I_TargetChecks::targetOpStart
 */
GTI_ANALYSIS_RETURN TargetChecks::targetOpStart(
        MustRMAId rmaId) {
    I_TargetRMAOpPersistent* op = myRMAMod->getPersistentTargetRMAOp(rmaId);

#ifdef MUST_DEBUG
    std::stringstream msg;
    msg << "TargetRMAOp STARTED: ";
    msg << "callId: " << rmaId;
    msg << ", pId: " << op->getPId();
    msg << ", lId: " << op->getLId();
    msg << ", isStore: " << op->isStore();
    msg << ", win: " << op->getWin();
    msg << ", target: " << op->getTarget();
    msg << ", annotation address: " << op->getFiber();
    msg << std::endl;
    std::cout << msg.str();
#endif
    
    return GTI_ANALYSIS_SUCCESS;
}

/**
 * @see I_TargetChecks::targetOpComplete
 */
GTI_ANALYSIS_RETURN TargetChecks::targetOpComplete(
        MustParallelId pId,
        MustLocationId lId,
        MustRMAId* rmaId,
        int rmaIdLen) {
#ifdef MUST_DEBUG
    std::cout << "=== TARGET OPS COMPLETED ===" << std::endl;
#endif
    // get current fiber to switch back at the end
    myTSanMod->annotateHappensBefore(pId, lId, rmaId);
    void* curFiber = myTSanMod->getCurrentFiber();

    for (int i = 0; i < rmaIdLen; ++i) {
        I_TargetRMAOp* op = myRMAMod->getTargetRMAOp(rmaId[i]);

#ifdef MUST_DEBUG
        std::stringstream msg;
        msg << "TargetRMAOp COMPLETED: ";
        msg << "callId: " << rmaId[i];
        msg << ", pId: " << op->getPId();
        msg << ", lId: " << op->getLId();
        msg << ", isStore: " << op->isStore();
        msg << ", isLocked: " << op->isLocked();
        msg << ", win: " << op->getWin();
        msg << ", win id: " << op->getWinId();
        msg << ", target: " << op->getTarget();
        msg << ", annotation address: " << op->getFiber();
        msg << ", return addr: " << op->getReturnAddr();
        msg << ", function addr: " << op->getFunctionAddr();
        msg << std::endl;
        std::cout << msg.str();
#endif
        // select fiber
        void* tempFiber;

        if (myFiberPool.find(op->getOrigin()) != myFiberPool.end()) {
            // fiber already created for origin rank
            tempFiber = myFiberPool[op->getOrigin()];
        } else {
            // create new fiber for origin rank
            myTSanMod->annotateIgnoreSyncBegin();
            tempFiber = myTSanMod->createFiber(0);
            myFiberPool.insert(std::make_pair(op->getOrigin(), tempFiber));
            myTSanMod->annotateIgnoreSyncEnd();
        }

        // switch to fiber of operation without synchronization (flag: 1)
        myTSanMod->switchToFiber(tempFiber, 1);

        // load corresponding vector clock
        myTSanMod->annotateHappensAfter(pId, lId, op->getFiber());

        // acquire lock if operation is locked and there is a local lock annotation
        const bool acquireLock = op->isLocked() && myRMAMod->getWinLockAddr(op->getWinId());
        if (acquireLock) {
            void* lock = myRMAMod->getWinLockAddr(op->getWinId());
            myTSanMod->annotateRWLockAcquired(op->getPId(), op->getLId(), lock, true);
        }

        // annotate atomic accesses separately
        if (op->isAtomic()) {
            MustMpiDatatypePredefined baseType = extractBasetype(op->getTargetDatatype());
            annotateAtomicAccesses(op->getPId(), op->getLId(), op->isStore(), op->getMemIntervals(), baseType);
        } else {
            annotateMemAccess(op->getPId(), op->getLId(), op->getMemIntervals(), op->isStore(), op->getReturnAddr(), op->getFunctionAddr());
        }

        // release acquired lock
        if (acquireLock) {
            void* lock = myRMAMod->getWinLockAddr(op->getWinId());
            myTSanMod->annotateRWLockReleased(op->getPId(), op->getLId(), lock, true);
        }
    }

    // switch to current thread *with* synchronization
    // (memory accesses afterwards do *not* conflict with *this* RMA call)
    myTSanMod->annotateHappensBefore(pId, lId, rmaId);
    myTSanMod->switchToFiber(curFiber, 0);
    myTSanMod->annotateHappensAfter(pId, lId, rmaId);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// annotateAtomicAccesses
//=============================
void TargetChecks::annotateAtomicAccesses(
        MustParallelId pId,
        MustLocationId lId,
        bool isStore,
        const MustMemIntervalListType& memIntervals,
        MustMpiDatatypePredefined baseType) {
    myTSanMod->annotateFuncEntry(pId, lId);

#ifdef MUST_DEBUG
    for (MustMemIntervalListType::iterator it = memIntervals.begin(); it != memIntervals.end(); ++it) {
        std::stringstream msg;
        msg << "stride: " << it->stride;
        msg << ", blocksize: " << it->blocksize;
        msg << ", count: " << it->count;
        msg << ", repetition: " << it->repetition;
        msg << std::endl;
        std::cout << msg.str();
    }
#endif

    switch (baseType) {
        // TODO: check for problems in shadow memory when annotating bytewise accesses
        case MUST_MPI_CHAR: // 8 bit
        case MUST_MPI_SIGNED_CHAR:
        case MUST_MPI_UNSIGNED_CHAR:
        case MUST_MPI_BYTE:
        case MUST_MPI_PACKED:
        case MUST_MPI_INT8_T:
        case MUST_MPI_UINT8_T:
        case MUST_MPI_C_BOOL:
            for (MustMemIntervalListType::iterator it = memIntervals.begin();
                    it != memIntervals.end(); ++it) {
                for (int i = 0; i < it->count; ++i) {
                    MustAddressType start = it->baseAddress + (it->stride * i);
                    MustAddressType end = start + it->blocksize;
                    for (MustAddressType curAddr = start; curAddr < end; curAddr += 1) {
                        if (isStore)
                            myTSanMod->annotateAtomic8Store((unsigned char*) curAddr);
                        else
                            myTSanMod->annotateAtomic8Load((unsigned char*) curAddr);
                    }
                }
            }

            break;

        case MUST_MPI_SHORT: // 16 bit
        case MUST_MPI_UNSIGNED_SHORT:
        case MUST_MPI_WCHAR:
        case MUST_MPI_INT16_T:
        case MUST_MPI_UINT16_T:
            for (MustMemIntervalListType::iterator it = memIntervals.begin();
                    it != memIntervals.end(); ++it) {
                for (int i = 0; i < it->count; ++i) {
                    MustAddressType start = it->baseAddress + (it->stride * i);
                    MustAddressType end = start + it->blocksize;
                    for (MustAddressType curAddr = start; curAddr < end; curAddr += 2) {
                        if (isStore)
                            myTSanMod->annotateAtomic16Store((unsigned short*) curAddr);
                        else
                            myTSanMod->annotateAtomic16Load((unsigned short*) curAddr);
                    }
                }
            }

            break;


        case MUST_MPI_INT: // 32 bit
        case MUST_MPI_UNSIGNED:
        case MUST_MPI_LONG:
        case MUST_MPI_UNSIGNED_LONG:
        case MUST_MPI_FLOAT:
        case MUST_MPI_INT32_T:
        case MUST_MPI_UINT32_T:
            for (MustMemIntervalListType::iterator it = memIntervals.begin();
                    it != memIntervals.end(); ++it) {
                for (int i = 0; i < it->count; ++i) {
                    MustAddressType start = it->baseAddress + (it->stride * i);
                    MustAddressType end = start + it->blocksize;
                    for (MustAddressType curAddr = start; curAddr < end; curAddr += 4) {
                        if (isStore)
                            myTSanMod->annotateAtomic32Store((unsigned int*) curAddr);
                        else
                            myTSanMod->annotateAtomic32Load((unsigned int*) curAddr);
                    }
                }
            }

            break;

        case MUST_MPI_LONG_LONG: // 64 bit
        case MUST_MPI_LONG_LONG_INT:
        case MUST_MPI_LONG_DOUBLE:
        case MUST_MPI_UNSIGNED_LONG_LONG:
        case MUST_MPI_DOUBLE:
        case MUST_MPI_INT64_T:
        case MUST_MPI_UINT64_T:
            for (MustMemIntervalListType::iterator it = memIntervals.begin();
                    it != memIntervals.end(); ++it) {
                for (int i = 0; i < it->count; ++i) {
                    MustAddressType start = it->baseAddress + (it->stride * i);
                    MustAddressType end = start + it->blocksize;
                    for (MustAddressType curAddr = start; curAddr < end; curAddr += 8) {
                        if (isStore)
                            myTSanMod->annotateAtomic64Store((unsigned long long*) curAddr);
                        else
                            myTSanMod->annotateAtomic64Load((unsigned long long*) curAddr);
                    }
                }
            }

            break;

        default:
            std::cout << "Error TargetChecks: Unsupported base type (id = " << baseType << ") for atomic annotation.";
            break;
    }
    myTSanMod->annotateFuncExit();
}


//=============================
// annotateMemAccess
//=============================
void TargetChecks::annotateMemAccess(
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
// extractBasetype
//=============================
MustMpiDatatypePredefined TargetChecks::extractBasetype(I_Datatype* datatype) {
    MustTypesigType typesig = datatype->getTypesig();

    // set type to first basetype in entry
    MustMpiDatatypePredefined type = typesig.front().second;

    for (MustTypesigType::iterator it = typesig.begin(); it != typesig.end(); ++it) {
        // check that all basetypes are the same
        if (type != it->second && type != MUST_MPI_LB && type != MUST_MPI_UB)
            return MUST_MPI_DATATYPE_UNKNOWN;
    }

    return type;
}


//=============================
// Destructor.
//=============================
TargetChecks::~TargetChecks (void)
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
