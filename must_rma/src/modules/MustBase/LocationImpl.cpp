/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file LocationImpl.cpp
 *       Implementation for the location id analysis interface.
 *
 *  @date 07.01.2010
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "MustDefines.h"
#include "PrefixedOstream.hpp"

#include "LocationImpl.h"

using namespace must;

mGET_INSTANCE_FUNCTION(LocationImpl)
mFREE_INSTANCE_FUNCTION(LocationImpl)
mPNMPI_REGISTRATIONPOINT_FUNCTION(LocationImpl)

//=============================
// Constructor
//=============================
LocationImpl::LocationImpl (const char* instanceName)
    : gti::ModuleBase<LocationImpl, I_LocationAnalysis, false> (instanceName),
      myGlobalMap (),
      myLocalMap (),
      myForwardedIds (),
      myEmptyInfo ()
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //No sub modules needed ...

    //Module data
    myEmptyInfo.callName = "";
    getWrapAcrossFunction("passLocationAcross", (GTI_Fct_t*)&myPassAcross);
}

//=============================
// Destructor
//=============================
LocationImpl::~LocationImpl ()
{
	xlock_safe_ptr(myGlobalMap)->clear ();
	xlock_safe_ptr(myLocalMap)->clear ();
	xlock_safe_ptr(myForwardedIds)->clear ();
}

//=============================
// registerLocation
//=============================
GTI_ANALYSIS_RETURN LocationImpl::registerLocation (
	MustParallelId pId,
	MustLocationId lId,
	const char* callName,
	int callNameLen,
	const void* codeptr,
	const char* fname,
	size_t fnameLen,
	const void* fbase
#ifdef ENABLE_STACKTRACE
	,
	int numStackLevels,
	int stackInfosLength,
	int indicesLength,
	int* infoIndices,
	const char* stackInfos
#endif
    )
{
    //Make sure we kill the upper 32 bit that hold the occurrence count, we do not care about that here!
    lId = (lId & 0x00000000FFFFFFFF);

#ifdef MUST_DEBUG
		must::cout << "DEBUG: Added location with id " << lId << " from parallel id " << pId << std::endl;
#endif

	//===Determine whether a local mapping for this location is present
	std::pair<MustParallelId, MustLocationId> idPair = std::make_pair (pId, lId);
        {
          auto s_safe_LocalMap = slock_safe_ptr(myLocalMap);
          auto lPos = s_safe_LocalMap->find(idPair);

          if (lPos != s_safe_LocalMap->end())
            // already an entry present, nothing todo (we silently assume that
            // the infos are both the same, hopefully this is usually true
            return GTI_ANALYSIS_SUCCESS;
        }

	//===Build the location info
	LocationInfo info;
    info.callName = callName;
	info.codeptr  = codeptr;
	info.fname    = fname;
	info.fbase    = fbase;

#ifdef ENABLE_STACKTRACE
	int level;
	int index = 0;
	for (level = 0; level < numStackLevels; level++)
	{
	    MustStackLevelInfo levelInfo;
	    for (int sub = 0; sub < 3; sub++)
	    {
	        int max = infoIndices[level*3 + sub];

	        char temp[1024];

	        int curr = 0;
	        while (index <= max && curr < 1024)
	        {
	            temp[curr] = stackInfos[index];
	            index++;
	            curr++;
	        }
	        temp[curr] = '\0'; //End (not necessary unleas larger 1024)

	        switch (sub)
	        {
	        case 0:
	            levelInfo.symName = temp;
	            break;
	        case 1:
	            levelInfo.fileModule = temp;
	            break;
	        case 2:
	            levelInfo.lineOffset = temp;
	            break;
	        }
	    }

	    info.stack.push_back (levelInfo);

	    //DEBUG
	    //must::cout << "Location:" <<  info.callName << " -> " << levelInfo.symName << "@" << levelInfo.fileModule << ":" << levelInfo.lineOffset << std::endl;
	}
#endif

	//===Is an equal global mapping already present ?
        GlobalMapType::const_iterator gPos;
        bool foundInGlobal = false;
        {
          auto s_safe_GlobalMap = slock_safe_ptr(myGlobalMap);
          gPos = s_safe_GlobalMap->find(lId);

          if (gPos != s_safe_GlobalMap->end())
            foundInGlobal = true;
        }
        if (foundInGlobal) {
          bool addToLocal = false;

		//==Already present, if it differs from the current entry we need to add it to local
		LocationInfo other = gPos->second;
		if ((other.callName != info.callName) || (other.codeptr != info.codeptr))
			addToLocal = true;

#ifdef ENABLE_STACKTRACE
		if (other.stack.size () != info.stack.size())
		    addToLocal = true;

		if (!addToLocal)
		{
		    std::list<MustStackLevelInfo>::iterator a, b;
		    for (a = other.stack.begin(), b = info.stack.begin(); a != other.stack.end(); a++, b++)
		    {
		        if (a->symName != b->symName || a->lineOffset != b->lineOffset)
		        {
		            addToLocal = true;
		            break;
		        }
		    }
		}
#endif

		if (addToLocal)
		{
                  xlock_safe_ptr(myLocalMap)
                      ->insert(std::make_pair(idPair, info));
                }
        } else {
          // Not in gobal yet -> add to global
          xlock_safe_ptr(myGlobalMap)->insert(std::make_pair(lId, info));
        }

        return GTI_ANALYSIS_SUCCESS;
}

//=============================
// getInfoForId
//=============================
const LocationInfo &LocationImpl::getInfoForId(MustParallelId pId,
                                               MustLocationId lId) {
  // Make sure we kill the upper 32 bit that hold the occurrence count, we do
  // not care about that here!
  lId = (lId & 0x00000000FFFFFFFF);

  //=== Is a local entry present -> use it
  std::pair<MustParallelId, MustLocationId> idPair = std::make_pair(pId, lId);
  auto s_safe_LocalMap = slock_safe_ptr(myLocalMap);
  LocalMapType::const_iterator lPos = s_safe_LocalMap->find(idPair);
  if (lPos != s_safe_LocalMap->end())
    return lPos->second;

  //=== Is a global entry present -> use it
  auto s_safe_GlobalMap = slock_safe_ptr(myGlobalMap);
  GlobalMapType::const_iterator gPos = s_safe_GlobalMap->find(lId);
  if (gPos != s_safe_GlobalMap->end())
    return gPos->second;

  //=== Ups, unknown location id, we return something in that case
  //        Probably empty string for call name is a good indicator for that.
  ////We currently do not warn about this, as the location reduction will
  ///actually lead to such cases /If we still want to warn we must use an extra
  ///interface call for the location reduction
  // static bool unknownWarned = false;
  // if (!unknownWarned)
  //	must::cerr << "Unknown location with id " << lId << " (parallel id " <<
  //pId << ") (This warning is only given once)" << std::endl; unknownWarned =
  // true;

  return myEmptyInfo;
}

//=============================
// getOccurenceCount
//=============================
int LocationImpl::getOccurenceCount (MustLocationId id)
{
    return (int)(id >> 32);
}

//=============================
// execCommand
//=============================
std::string LocationImpl::execCommand(std::string cmd) const
{
	const size_t max_buffer = 256;

	std::string data;
	FILE *stream = popen(cmd.c_str(), "r");
	if (stream) {
		char buffer[max_buffer];
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != NULL)
				data.append(buffer);
		pclose(stream);
	}
	return data;
}

//=============================
// toString
//=============================
std::string LocationImpl::toString (MustParallelId pId, MustLocationId lId)
{
	LocationInfo info = getInfoForId (pId, lId);

	std::string ret = "call " + info.callName + "\n";

	if (info.codeptr) {
		std::ostringstream retSS;
		retSS << "codeptr: " << info.codeptr << " file: " << info.fname  << " baseoffset: " << info.fbase << std::endl;
		ret += retSS.str();
		std::ostringstream cmd;
#ifdef __APPLE__
		cmd << "atos -o " << info.fname
			<< " -l " << info.fbase
			<< " " << ((void*) ((uint64_t)info.codeptr - 1)); // FIXME: codeptr contains return address, have to subtract 1 to get correct value (Is this really correct?)
#else
		cmd << "addr2line -e " << info.fname
			<< " -f -C -i "
			<< ((void*) ((uint64_t)info.codeptr - 1)); // FIXME: codeptr contains return address, have to subtract 1 to get correct value (Is this really correct?)
#endif
		ret += this->execCommand(cmd.str());
	}

	return ret;
}

//=============================
// passLocationToPlace
//=============================
GTI_RETURN LocationImpl::passLocationToPlace (
        MustParallelId pId,
        MustLocationId lId,
        int toPlaceId)
{
    //Make sure we kill the upper 32 bit that hold the occurrence count, we do not care about that here!
    lId = (lId & 0x00000000FFFFFFFF);

    //We didn't got the wrap across function, so this won't work!
    if (!myPassAcross)
        return GTI_ERROR;

    //Create key for our forwards knowledege
    ForwardKey key = std::make_pair(std::make_pair(pId, lId), toPlaceId);

    //If we already forwarded, all is good
    auto x_safe_ForwardedIds = xlock_safe_ptr(myForwardedIds);	
    if (x_safe_ForwardedIds->find (key) != x_safe_ForwardedIds->end())
        return GTI_SUCCESS;

    //Store that we now shipped it !
    x_safe_ForwardedIds->insert(key);

    //Find the right location
    const LocationInfo &toSendInfo = getInfoForId(pId, lId);

    //Create the character strings to use for the forwarding of
    //the location information
    /* IMPORTANT:  This must closely match whats in InitLocationId.cpp, thes use of different data structures requires some redundancy unfortunately */
#ifdef ENABLE_STACKTRACE
    char totalInfo[MUST_MAX_TOTAL_INFO_SIZE];
    int InfoIndices[MUST_MAX_NUM_STACKLEVELS*3];
    int maxtotalLen = MUST_MAX_TOTAL_INFO_SIZE -  MUST_MAX_NUM_STACKLEVELS*4;
    int totalLength = 0;
    int infoIndicesIndex = 0;

    std::list<MustStackLevelInfo>::const_iterator iter;
    for (iter = toSendInfo.stack.begin(); iter != toSendInfo.stack.end() && infoIndicesIndex<MUST_MAX_NUM_STACKLEVELS*3; iter++)
    {
        for (int piece = 0; piece < 3; piece++)
        {
            const char* info = NULL;

            switch (piece)
            {
            case 0: info = iter->symName.c_str();
            break;
            case 1: info = iter->fileModule.c_str();
            break;
            case 2: info = iter->lineOffset.c_str();
            break;
            }

            int i = 0;
            while (info && info[i] != '\0' && totalLength < maxtotalLen)
            {
                totalInfo[totalLength] = info[i];
                i++;
                totalLength++;
            }
            totalInfo[totalLength] = '\0';
            totalLength++;

            InfoIndices[infoIndicesIndex] = totalLength-1;
            infoIndicesIndex++;
        }
    }
#endif

    myPassAcross (
            pId,
            lId,
            toSendInfo.callName.c_str(), //we cast a const away here ... shouldn't hurt
            toSendInfo.callName.length()+1,
            toSendInfo.codeptr,
            (char*)(void*)toSendInfo.fname.c_str(), //we cast a const away here ... shouldn't hurt
            toSendInfo.fname.length()+1,
            toSendInfo.fbase,
#ifdef ENABLE_STACKTRACE
            infoIndicesIndex/3, /*Num stack levels*/
            totalLength, /*stack infos total length*/
            infoIndicesIndex, /*indicesLength*/
            InfoIndices, /*infoIndices*/
            totalInfo, /*StackInfos*/
#endif
            toPlaceId
        );

    return GTI_SUCCESS;
}

/*EOF*/
