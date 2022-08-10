/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file BlockingOp.cpp
 *       @see must::BlockingOp.
 *
 *  @date 08.08.2011
 *  @author Tobias Hilbrich, Mathias Korepkat, Joachim Protze
 */

#include "BlockingOp.h"
#include "BlockingState.h"

using namespace must;

//=============================
// Constructor
//=============================
BlockingOp::BlockingOp (
        BlockingState* state,
        MustParallelId pId,
        MustLocationId lId
)
: myState (state),
  myPId (pId),
  myLId (lId)
{
    myRank = myState->myPIdMod->getInfoForId(pId).rank;
}

//=============================
// Destructor
//=============================
BlockingOp::~BlockingOp (void)
{
    myState = NULL;
}

//=============================
// getIssuerRank
//=============================
int BlockingOp::getIssuerRank (void)
{
    return myRank;
}

//=============================
// needsSecondary
//=============================
bool BlockingOp::needsSecondary (void)
{
    return false;
}

//=============================
// getPId
//=============================
MustParallelId BlockingOp::getPId (void)
{
    return myPId;
}

//=============================
// getLId
//=============================
MustLocationId BlockingOp::getLId (void)
{
    return myLId;
}

//=============================
// isMatchingColl
//=============================
bool BlockingOp::isMatchingColl (MustCollCommType collId, I_Comm *comm)
{
    //Per default false
    return false;
}

//=============================
// registerSecondaryP2P
//=============================
void BlockingOp::registerSecondaryOp (BlockingOp* secondary)
{
    //Nothing to do
}

//=============================
// applyRequestToWait
//=============================
bool BlockingOp::applyP2PToWait (
        std::string label,
        P2PInfo *info,
        std::list<int> *outToRanks,
        std::list<std::string> *outLabels,
        std::list<std::pair<bool, std::pair<MustParallelId,MustLocationId> > > *outReferences,
        std::map<I_Comm*, std::string> &commLabels)
{
    std::stringstream fullLabel;
    fullLabel << label;

    //Find the communicator representative in the comm labels
    std::string commSymbol = "";
    std::map<I_Comm*, std::string>::iterator commIter;
    for (commIter = commLabels.begin(); commIter != commLabels.end(); commIter++)
    {
        if (info->comm->compareComms(commIter->first))
        {
            commSymbol = commIter->second;
            break;
        }
    }

    //Create the label
    if (label != "")
        fullLabel << ", comm=" << commSymbol;
    else
        fullLabel << " comm=" << commSymbol;

    fullLabel << ", tag=";

    if (info)
    {
        if (info->tag != this->myState->myConsts->getAnyTag())
            fullLabel << info->tag;
        else
            fullLabel << "MPI_ANY_TAG: " << info->tag;
    }

    if (!info->isWc)
    {
        outToRanks->push_back(info->target);
        if (outLabels) outLabels->push_back(fullLabel.str());
        if (outReferences) outReferences->push_back(std::make_pair(true, std::make_pair(info->pId, info->lId)));
    }
    else
    {
        I_Comm *comm = info->comm;
        I_GroupTable *table = comm->getGroup();

        //For intercomms we have to use the remote group!
        if (comm->isIntercomm())
            table = comm->getRemoteGroup();

        for (int i = 0; i < table->getSize(); i++)
        {
            int wRank;
            table->translate(i, &wRank);

            outToRanks->push_back(wRank);

            //We only add label and ref to the first arc to avoid cluttering
//            if (i == 0)
//            {
                if (outLabels) outLabels->push_back (fullLabel.str());
                if (outReferences) outReferences->push_back(std::make_pair(true, std::make_pair(info->pId, info->lId)));
//            }
//            else
//            {
//                if (outLabels) outLabels->push_back ("");
//                if (outReferences) outReferences->push_back(std::make_pair(false, std::make_pair(0,0)));
//            }
        }
    }

    return true;
}

//=============================
// copyQueuedOp
//=============================
I_Operation* BlockingOp::copyQueuedOp (void)
{
    return copy ();
}

//=============================
// isCollective
//=============================
bool BlockingOp::isCollective (void)
{
    return false;
}

//=============================
// waitsForASend
//=============================
bool BlockingOp::waitsForASend (int fromRank)
{
    return false;
}

//=============================
// waitsForAReceive
//=============================
bool BlockingOp::waitsForAReceive (int fromRank)
{
    return false;
}

/*EOF*/
