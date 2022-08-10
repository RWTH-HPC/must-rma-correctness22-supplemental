/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file QOpCommunicationP2PNonBlocking.cpp
 *       @see must::QOpCommunicationP2PNonBlocking.
 *
 *  @date 01.03.2013
 *  @author Tobias Hilbrich
 */

#include "QOpCommunicationP2PNonBlocking.h"

using namespace must;

//=============================
// QOpCommunicationP2PNonBlocking
//=============================
QOpCommunicationP2PNonBlocking::QOpCommunicationP2PNonBlocking (
                DWaitState *dws,
                MustParallelId pId,
                MustLocationId lId,
                MustLTimeStamp ts,
                I_CommPersistent *comm,
                bool isSend,
                int sourceTarget,
                bool isWc,
                MustSendMode mode,
                int tag,
                MustRequestType request
        )
 : QOpCommunicationP2P (dws, pId, lId, ts, comm, isSend, sourceTarget, isWc, mode, tag),
   myRequest(request)
{
    //Nothing to do
}

//=============================
// QOpCommunicationP2PNonBlocking
//=============================
QOpCommunicationP2PNonBlocking::~QOpCommunicationP2PNonBlocking (void)
{
    //Nothing to do
}

//=============================
// hasRequest
//=============================
bool QOpCommunicationP2PNonBlocking::hasRequest ()
{
    return true;
}

//=============================
// getRequest
//=============================
MustRequestType QOpCommunicationP2PNonBlocking::getRequest ()
{
    return myRequest;
}

//=============================
// printVariablesAsLabelString
//=============================
std::string QOpCommunicationP2PNonBlocking::printVariablesAsLabelString (void)
{
    std::stringstream stream;
    stream << "|request=" << myRequest;

    return QOpCommunicationP2P::printVariablesAsLabelString() + stream.str();
}

//=============================
// isNonBlockingP2P
//=============================
bool QOpCommunicationP2PNonBlocking::isNonBlockingP2P (void)
{
    return true;
}

/*EOF*/
