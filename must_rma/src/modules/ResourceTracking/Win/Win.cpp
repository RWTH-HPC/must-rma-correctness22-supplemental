/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file Win.cpp
 *       @see I_Win.
 *
 *  @date 26.04.2017
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat, Simon Schwitanski
 */

#include "Win.h"

using namespace must;

//=============================
// Win
//=============================
Win::Win ()
 : HandleInfoBase ("Win"),
   myKind (MUST_WIN_UNKNOWN),
   myMemoryModel(MUST_WIN_MEMORY_UNKNOWN),
   myComm (NULL),
   myCreationPId (0),
   myCreationLId (0),
   myContextId (0),
   myBase(0),
   myDispUnit(0)
{
    //Nothing to do
}

//=============================
// ~WinWin
//=============================
Win::~Win ()
{
    if (myComm)
        myComm->erase();
    myComm = NULL;
}

//=============================
// getComm
//=============================
I_CommPersistent* Win::getComm (void)
{
    return myComm;
}

//=============================
// getCommHandle
//=============================
MustCommType Win::getCommHandle (void)
{
    return myCommHandle;
}


//=============================
// getCreationPId
//=============================
MustParallelId Win::getCreationPId (void)
{
    return myCreationPId;
}

//=============================
// getCreationLId
//=============================
MustLocationId Win::getCreationLId (void)
{
    return myCreationLId;
}

//=============================
// printInfo
//=============================
bool Win::printInfo (
                std::stringstream &out,
                std::list<std::pair<MustParallelId,MustLocationId> > *pReferences)
{
    return true;
}

//=============================
// getResourceName
//=============================
std::string Win::getResourceName (void)
{
    return "Win";
}

//=============================
// getKind
//=============================
MUST_WIN_KIND Win::getKind (void)
{
    return myKind;
}

//=============================
// getMemoryModel
//=============================
MUST_WIN_MEMORY_MODEL Win::getMemoryModel (void)
{
    return myMemoryModel;
}

//=============================
// getBase
//=============================
MustAddressType Win::getBase (void) {
    return myBase;
}

//=============================
// getDispUnit
//=============================
int Win::getDispUnit (void) {
    return myDispUnit;
}

//=============================
// getContextId
//=============================
unsigned long long Win::getContextId (void) {
    return myContextId;
}

//=============================
// getMemIntervals
//=============================
MustMemIntervalListType& Win::getMemIntervals (void) {
    return myMemIntervals;
}

//=============================
// compareWins
//=============================
bool Win::compareWins (I_Win* other)
{
    if (other == NULL)
        return false;

    // windows should have the same communicator and the same context id
    return *getComm() == *(other->getComm()) && getContextId() == other->getContextId();
}

//=============================
// operator ==
//=============================
bool Win::operator == (I_Win &other)
{
	if (this == &other)
		return true;

	return compareWins(&other);
}

//=============================
// operator !=
//=============================
bool Win::operator != (I_Win &other)
{
	return ! (*this == other);
}

/*EOF*/
