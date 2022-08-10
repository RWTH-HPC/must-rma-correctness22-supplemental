/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file InitLocationId.cpp
 *       @see must::InitLocationId.
 *
 *  @date 24.04.2014
 *  @author Tobias Hilbrich
 */

#include <assert.h>
#include <dlfcn.h>

#include "GtiMacros.h"
#include "MustDefines.h"
#include "PrefixedOstream.hpp"
#include <assert.h>
#include <atomic>
#include <pnmpi.h>

#include "InitLocationId.h"

#ifdef BUILD_CALLPATH
#include "pnmpimod.h"
#include "callpath_module.h"
#endif

#ifdef BUILD_BACKWARD
#include "backward.hpp"
thread_local backward::TraceResolver must::InitLocationId::tr{};
#endif

using namespace must;

mGET_INSTANCE_FUNCTION(InitLocationId)
mFREE_INSTANCE_FUNCTION(InitLocationId)
mPNMPI_REGISTRATIONPOINT_FUNCTION(InitLocationId)

//=============================
// Constructor
//=============================
InitLocationId::InitLocationId (const char* instanceName)
    : gti::ModuleBase<InitLocationId, I_InitLocationId> (instanceName),
      myKnownLocations (),
#ifdef USE_CALLPATH
    uniqueLocationId(0)
#else
    uniqueLocationId(100000)
#endif
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_MODS_REQUIRED 1
    if (subModInstances.size() < NUM_MODS_REQUIRED)
    {
        must::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
        assert (0);
    }
    if (subModInstances.size() > NUM_MODS_REQUIRED)
    {
        for (std::vector<I_Module*>::size_type i = NUM_MODS_REQUIRED; i < subModInstances.size(); i++)
            destroySubModuleInstance (subModInstances[i]);
    }

    myPIdInit = (I_InitParallelId*) subModInstances[0];

    //Module data
    getWrapperFunction ("handleNewLocation", (GTI_Fct_t*)&myNewLocFct);

    if (!myNewLocFct)
    {
        must::cerr << "InitLocationId module could not find the \"handleNewLocation\" function and will not operate correctly as a result. Check the module mappings and specifications for this module and the function. Aborting." << std::endl;
        assert (0);
    }
}

GTI_ANALYSIS_RETURN InitLocationId::getUniqueLocationId (MustLocationId *pStorage)
{
    if (!pStorage)
        return GTI_ANALYSIS_FAILURE;
    *pStorage = uniqueLocationId++;
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// init
//=============================
GTI_ANALYSIS_RETURN InitLocationId::init (MustLocationId *pStorage, const char* callName, int callId)
{
    /* First, check if PnMPI provides the return address for this specific call.
     * It will point to the location, where the MPI call has been issued in the
     * application and can give more accurate information than just the function
     * name.
     *
     * NOTE: This method's execution only proceeds, if this feature is not
     *       available by PnMPI (as the compiler doesn't support it). */
#ifndef ENABLE_STACKTRACE
    void *codeptr;
    if (PNMPI_Service_GetReturnAddress(&codeptr) == PNMPI_SUCCESS)
        return this->initCodePtr(callName, pStorage, codeptr);
#endif


    if (!pStorage)
        return GTI_ANALYSIS_FAILURE;

    MustLocationId id; //result value
    uint32_t occCount = 0; //occurrence count of this callId

#if defined(BUILD_BACKWARD)
    LocationInfoImpl<uint64_t> thisLocation{};
    LocationInfo thisFullLocation{};
    thisFullLocation.callName = thisLocation.callName = callName;
    bool newStack = false;

    backward::StackTrace st;
    st.load_here(32);

    for (size_t i = 0; i < st.size(); ++i) {
      thisLocation.stack.push_back((uint64_t)st[i].addr);
    }

    static std::atomic<int> nextLocationId{0};
    KnownLocationsType::iterator pos;

    pos = myKnownLocations.find(callId);
    if (pos == myKnownLocations.end()) {
      newStack = true;
      id = nextLocationId++;
      // c-1) Its a new location
      occCount = 1;
      std::map<LocationInfoImpl<uint64_t>, MustLocationId> temp;
      temp.insert(std::make_pair(thisLocation, id));
      myKnownLocations.insert(
          std::make_pair(callId, std::make_pair(temp, occCount)));

    } else {
      // c-2) We have used this call id already (either new or old)
      std::map<LocationInfoImpl<uint64_t>, MustLocationId>::iterator callIdPos;

      callIdPos = pos->second.first.find(thisLocation);
      pos->second.second = pos->second.second + 1;
      occCount = pos->second.second;

      if (callIdPos == pos->second.first.end()) {
        id = nextLocationId++;
        // A new stack
        pos->second.first.insert(std::make_pair(thisLocation, id));
        newStack = true;
      } else {
        // A known stack
        id = callIdPos->second;
      }
    }

    //b) Build the information for this location (callName + stack)
    if (newStack) {
      tr.load_stacktrace(st);

      // first two entries are from backward + MUST itself, start with i = 3
      for (size_t i = 3; i < st.size(); ++i) {
        backward::ResolvedTrace trace = tr.resolve(st[i]);
        
        // Ignore MPI functions
        auto& object_func = trace.object_function;
        if ((object_func[0] == 'M' || object_func[0] == 'm') &&
            (object_func[1] == 'P' || object_func[1] == 'p') &&
            (object_func[2] == 'I' || object_func[2] == 'i') &&
            object_func[3] == '_')
            continue;

        // Ignore PnMPI functions
        if ((object_func[0] == 'N' || object_func[0] == 'n') &&
            (object_func[1] == 'Q' || object_func[1] == 'q') &&
            (object_func[2] == 'J' || object_func[2] == 'j') &&
            object_func[3] == '_')
            continue;

        // Ignore XMPI functions
        if ((object_func[0] == 'X' || object_func[0] == 'x') &&
            (object_func[1] == 'M' || object_func[1] == 'm') &&
            (object_func[2] == 'P' || object_func[2] == 'p') &&
            (object_func[3] == 'I' || object_func[3] == 'i') &&
            object_func[4] == '_')
            continue;

        // Ignore libc and entry point function
        if (object_func == "__libc_start_main" || object_func == "_start")
            break;
        
        // Ignore empty strings
        if (object_func.empty())
            continue;

        if (trace.source.function.empty()) {
          // Source information could not be read from the object
          thisFullLocation.stack.emplace_back(trace.object_filename,
                                              trace.object_function);
        } else {
          thisFullLocation.stack.emplace_back(
              trace.source.filename, trace.source.function,
              std::to_string(trace.source.line));
        }

        for(const auto& inlined_loc: trace.inliners) {
          thisFullLocation.stack.emplace_back(
              inlined_loc.filename, inlined_loc.function,
              std::to_string(inlined_loc.line));
        }

        if (object_func == "main" || object_func == "MAIN__")
          break;
      }
      createHandleNewLocationCall(id, callName, thisFullLocation);
    }

#elif defined(ENABLE_STACKTRACE)
    LocationInfo thisLocation{};
    thisLocation.callName = callName;
#ifdef BUILD_CALLPATH
    // a) For the callpath case: get the service function from the callpath
    // module
    static PNMPIMOD_Callpath_GetCallpath_t fct;
    static bool isInitialized = false;

    if (!isInitialized) {
      isInitialized = true;

      // find the callpath module
      PNMPI_modHandle_t module;
      int err = PNMPI_Service_GetModuleByName(PNMPI_MODULE_CALLPATH, &module);
      if (err != PNMPI_SUCCESS) {
        must::cerr << "Couldn't find module " PNMPI_MODULE_CALLPATH << std::endl;
        return GTI_ANALYSIS_FAILURE;
      }

      // get the service that will get us the actual call trace
      PNMPI_Service_descriptor_t service;
      err = PNMPI_Service_GetServiceByName(
          module, PNMPIMOD_Callpath_GetCallpath, "r", &service);
      if (err != PNMPI_SUCCESS) {
        must::cerr << "Couldn't find " PNMPIMOD_Callpath_GetCallpath " service!"
                  << std::endl;
        return GTI_ANALYSIS_FAILURE;
      }

      fct = reinterpret_cast<PNMPIMOD_Callpath_GetCallpath_t>(service.fct);
    }

    // b) Build the information for this location (callName + stack)
    std::list<StackInfo> sList = (*fct)();

    std::list<StackInfo>::iterator sIter;
    for (sIter = sList.begin(); sIter != sList.end(); sIter++) {
      MustStackLevelInfo levelInfo;
      levelInfo.symName = sIter->symName;
      levelInfo.fileModule = sIter->fileModule;
      levelInfo.lineOffset = sIter->lineOffset;
      thisLocation.stack.push_back(levelInfo);
    }
#endif // BUILD_CALLPATH || BUILD_BACKWARD
    //c) Search in the known locations
    static std::atomic<int> nextLocationId{0};
    KnownLocationsType::iterator pos;

    pos = myKnownLocations.find (callId);
    if (pos == myKnownLocations.end ())
    {
      id = nextLocationId++;
      // c-1) Its a new location
      occCount = 1;
      std::map<LocationInfo, MustLocationId> temp;
      temp.insert(std::make_pair(thisLocation, id));
      myKnownLocations.insert(
          std::make_pair(callId, std::make_pair(temp, occCount)));
      createHandleNewLocationCall(id, (char *)(void *)callName, thisLocation);
    }
    else
    {
        //c-2) We have used this call id already (either new or old)
        std::map<LocationInfo, MustLocationId>::iterator callIdPos;

        callIdPos = pos->second.first.find (thisLocation);
        pos->second.second = pos->second.second + 1;
        occCount = pos->second.second;

        if (callIdPos == pos->second.first.end())
        {
          id = nextLocationId++;
          // A new stack
          pos->second.first.insert(std::make_pair(thisLocation, id));
          createHandleNewLocationCall(id, (char *)(void *)callName,
                                      thisLocation);
        }
        else
        {
            //A known stack
            id = callIdPos->second;
        }
    }
#else 
    //Search in the known locations
    KnownLocationsType::iterator pos;
    id = callId;
    pos = myKnownLocations.find (callId);
    if (pos == myKnownLocations.end ())
    {
        //Its a new location
        occCount = 1;
        LocationInfo info;
        info.callName = callName;
        myKnownLocations.insert (std::make_pair (id, std::make_pair(info, occCount)));

        MustParallelId pId;
        myPIdInit->init(&pId);
        (*myNewLocFct) (pId, id, (char*)(void*)callName, info.callName.length()+1, NULL, "\0", 1, NULL);
    }
    else
    {
        pos->second.second = pos->second.second + 1;
        occCount = pos->second.second;
    }
#endif

    //Store it
    //Lower 32 bit represent the location identifier, upper 32bit represent occurrence count
    *pStorage = (id & 0x00000000FFFFFFFF) | ((uint64_t)occCount << 32);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// createHandleNewLocationCall
//=============================

#ifdef ENABLE_STACKTRACE
void InitLocationId::createHandleNewLocationCall(MustLocationId id,
                                                 const char *callName,
                                                 LocationInfo &location) {
  char totalInfo[MUST_MAX_TOTAL_INFO_SIZE];
  int InfoIndices[MUST_MAX_NUM_STACKLEVELS * 3];
  int maxtotalLen = MUST_MAX_TOTAL_INFO_SIZE - MUST_MAX_NUM_STACKLEVELS * 4;
  int totalLength = 0;
  int infoIndicesIndex = 0;

  std::list<MustStackLevelInfo>::iterator iter;
  for (iter = location.stack.begin();
       iter != location.stack.end() &&
       infoIndicesIndex < MUST_MAX_NUM_STACKLEVELS * 3;
       iter++) {
    for (int piece = 0; piece < 3; piece++) {
      const char *info = NULL;

      switch (piece) {
      case 0:
        info = iter->symName.c_str();
        break;
      case 1:
        info = iter->fileModule.c_str();
        break;
      case 2:
        info = iter->lineOffset.c_str();
        break;
      }

      int i = 0;
      while (info && info[i] != '\0' && totalLength < maxtotalLen) {
        totalInfo[totalLength] = info[i];
        i++;
        totalLength++;
      }
      totalInfo[totalLength] = '\0';
      totalLength++;

      InfoIndices[infoIndicesIndex] = totalLength - 1;
      infoIndicesIndex++;
    }
  }

  MustParallelId pId;
  myPIdInit->init(&pId);
  (*myNewLocFct)(pId, id, callName, location.callName.length() + 1,
                 NULL,
                 "\0",
                 1,
                 NULL,
                 infoIndicesIndex / 3, /*Num stack levels*/
                 totalLength,          /*stack infos total length*/
                 infoIndicesIndex,     /*indicesLength*/
                 InfoIndices,          /*infoIndices*/
                 totalInfo);           /*StackInfos*/
}
#endif


//=============================
// initCodePtr
//=============================
GTI_ANALYSIS_RETURN InitLocationId::initCodePtr(
    const char* callname,
    MustLocationId* pStorage,
    const void* codeptr_ra)
{
    assert(pStorage);
    uint32_t occCount = 0;

    /* Generate the location identifier from the code pointer by simply casting
     * it to an integer. Then, check if this code pointer has already been
     * looked up or look it up now. */
    uint64_t lId = (MustLocationId)codeptr_ra;
    if (myKnownLocations.find(lId) == myKnownLocations.end())
    {
        /* Get the address information for the process currently running, so the
         * code pointer can be resolved later by the location implementation
         * module. If this operation fails for any reason, we can't proceed and
         * return an error. */
        Dl_info info;
        if (dladdr((void*)codeptr_ra, &info) == 0)
            return GTI_ANALYSIS_FAILURE;

        /* Insert the location information into the module's cache map, so later
         * invocations for the same code pointer don't need to be looked up
         * again. */
        LocationInfo loc_info;
        loc_info.callName = std::string(callname);
        loc_info.codeptr = codeptr_ra;
        loc_info.fname   = info.dli_fname;
        loc_info.fbase   = info.dli_fbase;

        myKnownLocations.insert(std::make_pair(
            lId,
            std::make_pair(loc_info, occCount)
        ));

        /* Distribute the new location information in the GTI tree, so all other
         * modules can access this information on any rank in the tree (e.g. for
         * printing the message). */
        MustParallelId pId;
        myPIdInit->init(&pId);
        (*myNewLocFct) (
            pId,
            lId,
            (char*)(void*)callname,
            strlen(callname)+1,
            codeptr_ra,
            info.dli_fname,
            strlen(info.dli_fname) + 1,
            info.dli_fbase
        );
    }

    /* Return the given location identifier and indicate success. The location
     * identifier is translated from the code pointer above. */
    *pStorage = lId;
    return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
