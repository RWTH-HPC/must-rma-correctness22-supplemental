/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file QOpCommunication.cpp
 *       @see must::QOpCommunication.
 *
 *  @date 28.02.2013
 *  @author Tobias Hilbrich
 */

#include "QOpCommunication.h"
#include "DWaitState.h"

using namespace must;

//=============================
// QOpCommunication
//=============================
QOpCommunication::QOpCommunication (
        DWaitState *dws,
        MustParallelId pId,
        MustLocationId lId,
        MustLTimeStamp ts,
        I_CommPersistent *comm
)
: QOp (dws, pId, lId, ts),
  myComm(comm)
{

}

//=============================
// ~QOpCommunication
//=============================
QOpCommunication::~QOpCommunication (void)
{
    if (myComm)
        myComm->erase();
    myComm = NULL;
}

//=============================
// getComm
//=============================
I_Comm* QOpCommunication::getComm (void)
{
    return myComm;
}

//=============================
// printVariablesAsLabelString
//=============================
std::string QOpCommunication::printVariablesAsLabelString (void)
{
    std::stringstream stream;
    if (myComm)
    {
        stream << "|comm=";
        if (myComm->isPredefined())
            stream << "MPI_COMM_WORLD"; //not exactly true, but this is for debuging purposes
        else
            stream << myState->getLocationlIdAnalysis()->getInfoForId(myComm->getCreationPId(), myComm->getCreationLId()).callName;
    }
    return QOp::printVariablesAsLabelString() + stream.str();
}

//=============================
// getUsedComms
//=============================
std::list<I_Comm*> QOpCommunication::getUsedComms (void)
{
    std::list<I_Comm*> ret;

    ret.push_back(myComm);

    return ret;
}

//=============================
// hasRequest
//=============================
bool QOpCommunication::hasRequest (void)
{
    return false;
}

//=============================
// getRequest
//=============================
MustRequestType QOpCommunication::getRequest (void)
{
    return 0; /*Invalid value*/
}

/*EOF*/
