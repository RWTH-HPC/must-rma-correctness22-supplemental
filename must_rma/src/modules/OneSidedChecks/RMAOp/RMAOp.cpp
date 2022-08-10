/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file RMAOp.cpp
 *
 *  @date 19.06.2017
 *  @author Simon Schwitanski
 */

#include "RMAOp.h"
#include "AppThrAnn.h"

using namespace must;

//=============================
// Constructor
//=============================
RMAOp::RMAOp(void):
    HandleInfoBase("RMAOp"),
    myPId(0),
    myLId(0),
    myOrigin(-1),
    myTarget(-1),
    myIsStore(false),
    myMemIntervals(),
    myWin(NULL),
    myWinId(0),
    myRMAEpoch(0),
    myFiber(NULL),
    myReturnAddr(NULL),
    myFunctionAddr(NULL) {
        // Nothing to do
}

//=============================
// Destructor 
//=============================
RMAOp::~RMAOp(void) {
    if (myWin)
        myWin->erase();
    myWin = NULL;

    // TODO: Use fiber pool?
    // put annotation address back to data pool
    /*if (myAnnAddr)
        delete static_cast<AnnData*>(myAnnAddr);
    myAnnAddr = NULL;*/
}

//=============================
// getPId
//=============================
MustParallelId RMAOp::getPId(void) {
    return myPId;
}

//=============================
// getLId
//=============================
MustLocationId RMAOp::getLId(void) {
    return myLId;
}

//=============================
// getOrigin
//=============================
int RMAOp::getOrigin(void) {
    return myOrigin;
}

//=============================
// getTarget
//=============================
int RMAOp::getTarget(void) {
    return myTarget;
}

//=============================
// isStore
//=============================
bool RMAOp::isStore(void) {
    return myIsStore;
}

//=============================
// getMemIntervals
//=============================
MustMemIntervalListType RMAOp::getMemIntervals(void) {
    return myMemIntervals;
}

//=============================
// getWin
//=============================
I_Win* RMAOp::getWin(void) {
    return myWin;
}

//=============================
// getWinId
//=============================
MustWinType RMAOp::getWinId(void) {
    return myWinId;
}

//=============================
// getRMAEpoch
//=============================
int RMAOp::getRMAEpoch(void) {
    return myRMAEpoch;
}

//=============================
// getWinAccessMode
//=============================
MustWinAccessMode RMAOp::getWinAccessMode(void) {
    return myWinAccessMode;
}

//=============================
// getFiber
//=============================
void* RMAOp::getFiber(void) {
    return myFiber;
}

//=============================
// setFiber
//=============================
void RMAOp::setFiber(void* fiber) {
    myFiber = fiber;
}


//=============================
// getReturnAddr
//=============================
void* RMAOp::getReturnAddr(void) {
    return myReturnAddr;
}


//=============================
// getFunctionAddr
//=============================
void* RMAOp::getFunctionAddr(void) {
    return myFunctionAddr;
}


//=============================
// printInfo
//=============================
bool RMAOp::printInfo (
                std::stringstream &out,
                std::list<std::pair<MustParallelId,MustLocationId> > *pReferences)
{
    return true;
}

//=============================
// getResourceName
//=============================
std::string RMAOp::getResourceName (void)
{
    return "RMAOp";
}

