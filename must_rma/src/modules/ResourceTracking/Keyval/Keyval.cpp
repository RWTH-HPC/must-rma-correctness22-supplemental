/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file Keyval.h
 *       @see Keyval.
 *
 *  @date 19.07.2011
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#include "Keyval.h"

#include <sstream>

using namespace must;

//=============================
// Constructor
//=============================
Keyval::Keyval ()
 : HandleInfoBase ("Keyval"),
   myPredefined (MUST_MPI_KEY_TAG_UB),
   myPredefinedName (""),
   myIsNull (true),
   myIsPredefined (false),
   myCreationPId (0),
   myCreationLId (0)
{
    //Nothing to do
}

//=============================
// Keyval
//=============================
Keyval::Keyval (MustMpiKeyvalPredefined predefined, std::string predefinedName)
:  HandleInfoBase ("Keyval"),
   myPredefined (predefined),
   myPredefinedName (predefinedName),
   myIsNull (false),
   myIsPredefined (true),
   myCreationPId (0),
   myCreationLId (0)
{
    //Nothing to do
}

//=============================
// ~Keyval
//=============================
Keyval::~Keyval ()
{
    //Nothing to do
}

//=============================
// isNull
//=============================
bool Keyval::isNull (void)
{
    return myIsNull;
}

//=============================
// isPredefined
//=============================
bool Keyval::isPredefined (void)
{
    return myIsPredefined;
}

//=============================
// getCreationPId
//=============================
MustParallelId Keyval::getCreationPId (void)
{
    return myCreationPId;
}

//=============================
// getCreationLId
//=============================
MustLocationId Keyval::getCreationLId (void)
{
    return myCreationLId;
}

//=============================
// getPredefinedInfo
//=============================
MustMpiKeyvalPredefined Keyval::getPredefinedInfo (void)
{
    return myPredefined;
}

//=============================
// getPredefinedName
//=============================
std::string Keyval::getPredefinedName (void)
{
    return myPredefinedName;
}

//=============================
// printInfo
//=============================
bool Keyval::printInfo (
        std::stringstream &out,
        std::list<std::pair<MustParallelId,MustLocationId> > *pReferences)
{
    //Is Null
    if(myIsNull)
    {
        out << "MPI_KEYVAL_INVALID";
        return true;
    }

    //Is Predefined
    if (myIsPredefined)
    {
        out << myPredefinedName;
        return true;
    }

    //A user defined key
    pReferences->push_back(std::make_pair (myCreationPId, myCreationLId));
    out << "Key created at reference  "<< pReferences->size();

    return true;
}

//=============================
// getResourceName
//=============================
std::string Keyval::getResourceName (void)
{
    return "Keyval";
}

/*EOF*/
