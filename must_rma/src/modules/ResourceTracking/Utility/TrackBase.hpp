/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TrackBase.hpp
 *       @see MUST::TrackBase.
 *
 *  @date 10.02.2011
 *  @author Tobias Hilbrich
 */

#include "PrefixedOstream.hpp"

//=============================
// Constructor
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::TrackBase (const char* instanceName)
: ModuleBase<SUPER, INTERFACE, false> (instanceName),
  myNullValue (0),
  myNullInfo (NULL),
  myPredefineds (),
  myUserHandles (),
  myLastQuery (),
  myRemoteRes (),
  myPIdMod (NULL),
  myLIdMod (NULL),
  myFurtherMods ()
  {
    HandleInfoBase::subscribeTracker();

	//create sub modules
	std::vector<I_Module*> subModInstances;
	subModInstances = ModuleBase<SUPER, INTERFACE, false>::createSubModuleInstances ();

	//handle sub modules
	if (subModInstances.size() < 2)
	{
		must::cerr << "ERROR: " << __FILE__ << "@" << __LINE__ << " needs one sub module as parallel id module and one as location if module." << std::endl;
		assert (0);
	}
	myFurtherMods.resize(subModInstances.size()-2);
	for (std::vector<I_Module*>::size_type i = 2; i < subModInstances.size(); i++)
	{
		myFurtherMods[i-2] = subModInstances[i];
	}

	myPIdMod = (I_ParallelIdAnalysis*) subModInstances[0];
	myLIdMod = (I_LocationAnalysis*) subModInstances[1];

	//initialize
        myLastQuery = slock_safe_ptr(myUserHandles)->end();
  }

//=============================
// Destructor
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::~TrackBase (void)
{
    //Notify HandleInfo Base that no frees should be passed across anymore
    HandleInfoBase::disableFreeForwardingAcross();

	//destroy ParallelId module
	if (myPIdMod)
		ModuleBase<SUPER, INTERFACE, false>::destroySubModuleInstance ((I_Module*) myPIdMod);
	myPIdMod = NULL;

	//destroy LocationlId module
	if (myLIdMod)
	    ModuleBase<SUPER, INTERFACE, false>::destroySubModuleInstance ((I_Module*) myLIdMod);
	myLIdMod = NULL;

	//destroy any other modules
	for (std::vector<I_Module*>::size_type i = 0; i < myFurtherMods.size(); i++)
	{
		if (myFurtherMods[i])
			ModuleBase<SUPER, INTERFACE, false>::destroySubModuleInstance (myFurtherMods[i]);
		myFurtherMods[i] = NULL;
	}
	myFurtherMods.clear();

	freeHandleMaps ();

	HandleInfoBase::unsubscribeTracker();
}

//=============================
// addPredefineds
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
GTI_ANALYSIS_RETURN TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::addPredefineds (
        MustParallelId pId,
        HANDLE_TYPE nullValue,
        int numPredefs,
        int* predefinedIds,
        HANDLE_TYPE* predefinedValues
)
{
    return addPredefineds(pId2Rank(pId), nullValue, numPredefs, predefinedIds, predefinedValues);
}
//=============================
// addPredefineds
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
GTI_ANALYSIS_RETURN TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::addPredefineds (
        int rank,
		HANDLE_TYPE nullValue,
		int numPredefs,
		int* predefinedIds,
		HANDLE_TYPE* predefinedValues
)
{
  /*TODO: think about thread-safe initialisation*/
  static std::atomic<bool> wasAdded{false};
  FULL_INFO *info;
  if (wasAdded) {
    if (myNullValue != nullValue) {
      xlock_safe_ptr(myNullValues)->insert(std::make_pair(rank, nullValue));
    }

    auto x_safe_UserHandles = xlock_safe_ptr(myUserHandles);
    auto x_safe_Predefineds = xlock_safe_ptr(myPredefineds);
    auto x_safe_PredefinedMap = xlock_safe_ptr(myPredefinedMap);
    for (int i = 0; i < numPredefs; i++) {
      typename PredefinedMap::iterator predef =
          x_safe_PredefinedMap->find(predefinedIds[i]);
      if (predef == x_safe_PredefinedMap->end()) { // should not happen?
        // assert(0);
        info = createPredefinedInfo(predefinedIds[i], predefinedValues[i]);
        x_safe_Predefineds->insert(std::make_pair(predefinedValues[i], info));
        x_safe_PredefinedMap->insert(std::make_pair(
            predefinedIds[i], std::make_pair(predefinedValues[i], info)));
      } else if (predef->second.first != predefinedValues[i]) {
        x_safe_UserHandles->insert(std::make_pair(
            std::make_pair(rank, predefinedValues[i]), predef->second.second));
        if (predef->second.second) {
          predef->second.second->copy();
        }
      }
    }
  } else {
    // store the null value
    myNullValue = nullValue;
    // enough to have one predefined info object?
    myNullInfo = createPredefinedInfo(0, nullValue);
    auto x_safe_Predefineds = xlock_safe_ptr(myPredefineds);
    auto x_safe_PredefinedMap = xlock_safe_ptr(myPredefinedMap);
    // store the other predefined values
    for (int i = 0; i < numPredefs; i++) {
      info = createPredefinedInfo(predefinedIds[i], predefinedValues[i]);
      x_safe_Predefineds->insert(std::make_pair(predefinedValues[i], info));
      x_safe_PredefinedMap->insert(std::make_pair(
          predefinedIds[i], std::make_pair(predefinedValues[i], info)));
    }
  }

  wasAdded = true;
  return GTI_ANALYSIS_SUCCESS;
}

//=============================
// findUserHandle
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE,
          typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
typename TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER,
                   INTERFACE>::HandleMap::const_iterator
TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER,
          INTERFACE>::findUserHandle(MustParallelId pId, HANDLE_TYPE handle) {
  return findUserHandle(pId2Rank(pId), handle);
}

//=============================
// findUserHandle
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE,
          typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
typename TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER,
                   INTERFACE>::HandleMap::const_iterator
TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER,
          INTERFACE>::findUserHandle(int rank, HANDLE_TYPE handle) {
  // Look at last query (avoid the search if that was already the right request)
  if (myLastQuery != slock_safe_ptr(myUserHandles)->end() &&
      myLastQuery->first.first == rank && myLastQuery->first.second == handle)
    return myLastQuery;

  myLastQuery =
      slock_safe_ptr(myUserHandles)->find(std::make_pair(rank, handle));

  return myLastQuery;
}

//=============================
// getHandleInfo
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
FULL_INFO* TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::getHandleInfo (MustParallelId pId, HANDLE_TYPE handle)
{
    return getHandleInfo (pId2Rank(pId), handle);
}

//=============================
// getHandleInfo
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
FULL_INFO* TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::getHandleInfo (int rank, HANDLE_TYPE handle)
{
    FULL_INFO* ret;

    // is it MPI_handle_NULL ?
    {
      auto s_safe_NullValues = slock_safe_ptr(myNullValues);
      auto value = s_safe_NullValues->end();
      if (!s_safe_NullValues->empty() &&
          (value = s_safe_NullValues->find(rank)) !=
              s_safe_NullValues->end()) { // rank specific NULL value available
        if (value->second == handle) {
          return myNullInfo;
        }
      } else if (handle == myNullValue) { // global NULL value
        return myNullInfo;
      }
    }

    auto s_safe_UserHandles = slock_safe_ptr(myUserHandles);
    //Is it a user defined type ?
    if (myLastQuery == s_safe_UserHandles->end() ||
        myLastQuery->first.first != rank ||
        myLastQuery->first.second != handle) {
      myLastQuery = s_safe_UserHandles->find(std::make_pair(rank, handle));
    }

    if (myLastQuery == s_safe_UserHandles->end()) {
      // If not a user type-> it must a predefined

      auto s_safe_Predefineds = slock_safe_ptr(myPredefineds);
      typename PredefinedInfos::const_iterator prePos =
          s_safe_Predefineds->find(handle);

      if (prePos == s_safe_Predefineds->end())
        return NULL;

      ret = prePos->second;
    } else {
      ret = myLastQuery->second;
    }

    return ret;
}

//=============================
// isAlreadyKnown
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
bool TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::isAlreadyKnown (
		MustParallelId pId, HANDLE_TYPE handle)
{
#ifdef MUST_DEBUG
    //Known as non-predefined request ?
    {
      auto s_safe_UserHandles = slock_safe_ptr(myUserHandles);
      typename HandleMap::const_iterator pos =
          s_safe_UserHandles->find(std::make_pair(pId2Rank(pId), handle));
      if (pos != s_safe_UserHandles->end()) {
        // This is more of an internal error, we log it directly to must::cerr
        must::cerr << "Error: in " << __FILE__ << __LINE__
                  << " tried to add a user handle that is still present, "
                     "implementation error in MPI or MUST."
                  << std::endl;
        return true;
      }
    }

    //Is null
    {
      auto s_safe_NullValues = slock_safe_ptr(myNullValues);
      auto value = s_safe_NullValues->end();
      if ((!s_safe_NullValues->empty() &&
           (value = s_safe_NullValues->find(pId2Rank(pId))) !=
               s_safe_NullValues->end() &&
           value->second == handle) ||
          handle == myNullValue) {
        // This is more of an internal error, we log it directly to must::cerr
        must::cerr << "Error: in " << __FILE__ << __LINE__
                  << " tried to add a handle that had the value of the null "
                     "handle, implementation error in MPI or MUST."
                  << std::endl;
        return true;
      }
    }

    {
	//Is it a predefined value
        auto s_safe_Predefineds = slock_safe_ptr(myPredefineds);
        typename PredefinedInfos::const_iterator prePos =
            s_safe_Predefineds->find(handle);
        if (prePos != s_safe_Predefineds->end()) {
          // This is more of an internal error, we log it directly to must::cerr
          must::cerr << "Error: in " << __FILE__ << __LINE__
                    << " tried to add a user handle that has the value of a "
                       "predefined handle, implementation error in MPI or MUST."
                    << std::endl;
          return true;
        }
    }
#endif /*MUST_DEBUG*/

    return false;
}

//=============================
// submitUserHandle
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
bool TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::submitUserHandle (
        MustParallelId pId, HANDLE_TYPE handle, FULL_INFO* handleInfo)
{
    return submitUserHandle (pId2Rank(pId), handle, handleInfo);
}

//=============================
// submitUserHandle
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
bool TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::submitUserHandle (
        int rank,
        HANDLE_TYPE handle,
        FULL_INFO* handleInfo)
{
    auto x_safe_UserHandles = xlock_safe_ptr(myUserHandles);

    std::pair<typename HandleMap::const_iterator, bool> ret =
        x_safe_UserHandles->insert(
            std::make_pair(std::make_pair(rank, handle), handleInfo));

    if (!ret.second)
    {
        //Ok, there is already a handle with same value in there! We need to overwrite
        //Kill old
        x_safe_UserHandles->erase (ret.first);

#ifdef MUST_DEBUG
        //Thats rather weird, we should warn!
        must::cout << "Warning: a user handle was added that was already in our user handle map, we overwrote the old one with the new one, but should that really happen? " << __FILE__<<":"<<__LINE__<<std::endl;
#endif /*MUST_DEBUG*/

        //Now again
        ret = x_safe_UserHandles->insert(
                std::make_pair(
                        std::make_pair(rank, handle),
                        handleInfo
                )
        );
    }

    // refCount initialized with 1 in constructor
    myLastQuery = ret.first;
    return ret.second;
}

//=============================
// removeUserHandle
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
bool TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::removeUserHandle (
        MustParallelId pId, HANDLE_TYPE handle)
{
    return removeUserHandle (pId2Rank(pId), handle);
}

//=============================
// removeUserHandle
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
bool TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::removeUserHandle (
        int rank,
        HANDLE_TYPE handle)
{
  typename HandleMap::const_iterator pos = findUserHandle(rank, handle);
  auto x_safe_UserHandles = xlock_safe_ptr(myUserHandles);
  if (pos == x_safe_UserHandles->end())
    return false;

  // decrease MPI refCount (Remove from list if last mpi ref count was removed
  // of no info is present at all)
  if (!pos->second || pos->second->mpiErase()) {
    x_safe_UserHandles->erase(pos);
    myLastQuery = x_safe_UserHandles->end();
    }

    return true;
}

//=============================
// submitRemoteResource
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
bool TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::submitRemoteResource (
        int rank,
        MustRemoteIdType remoteId,
        bool hasHandle,
        HANDLE_TYPE handle,
        FULL_INFO* handleInfo)
{
    //If we have a handle we need to add it to the user handles as well
    if (hasHandle)
    {
        submitUserHandle (rank, handle, handleInfo);
    }

    //Now add it to the Remote resources
    RemoteIdentifier id = std::make_pair (rank, remoteId);
    RemoteResourceInfo info = std::make_pair (handleInfo, std::make_pair(hasHandle, handle));
    
    xlock_safe_ptr(myRemoteRes)->emplace(std::make_pair(id, info));

    return true;
}

//=============================
// removeRemoteHandle
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
bool TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::removeRemoteResource (
        int rank,
        MustRemoteIdType remoteId)
{
    RemoteIdentifier id = std::make_pair(rank, remoteId);
    auto x_safe_RemoteRes = xlock_safe_ptr(myRemoteRes);
    typename RemoteMap::iterator pos = x_safe_RemoteRes->find (id);

    if (pos == x_safe_RemoteRes->end())
        //Does not exists, should we warn?
        return false;

    bool hasHandle = pos->second.second.first;
    HANDLE_TYPE handle = pos->second.second.second;
    FULL_INFO* resource = pos->second.first;

    if (hasHandle)
    {
        //This removes the resource from the user handles AND it kills the resource (should have a MPI ref count of exactly 1)
        removeUserHandle (rank, handle);
    }
    else
    {
        //We need to do the mpi erase ourselves
        if (resource) resource->mpiErase();
    }

    //Delete from the remote resource map
    x_safe_RemoteRes->erase (pos);

    return true;
}

//=============================
// getRemoteIdInfo
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
FULL_INFO* TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::getRemoteIdInfo (
        int rank,
        MustRemoteIdType remoteId)
{
    RemoteIdentifier id = std::make_pair (rank, remoteId);
    auto s_safe_RemoteRes = slock_safe_ptr(myRemoteRes);
    typename RemoteMap::const_iterator pos = s_safe_RemoteRes->find(id);

    if (pos == s_safe_RemoteRes->end())
      return NULL; // No such remote resource exists

    return pos->second.first;
}

//=============================
// pId2Rank
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
int TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::pId2Rank (
		MustParallelId pId)
{
	return myPIdMod->getInfoForId(pId).rank;
}

//=============================
// createPredefinedInfo
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
FULL_INFO *
TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::createPredefinedInfo (int value, HANDLE_TYPE handle)
{
	//return an invalid info
    return NULL;
}

//=============================
// getUserHandles
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
std::list<std::pair<int, HANDLE_TYPE> >
TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::getUserHandles (void)
{
	typename std::list<std::pair<int, HANDLE_TYPE> > handles;

        auto s_safe_UserHandles = slock_safe_ptr(myUserHandles);
        typename HandleMap::const_iterator i;
        for (i = s_safe_UserHandles->begin(); i != s_safe_UserHandles->end();
             i++) {
          if (!this->isPredefined((I_INFO *)i->second))
            handles.push_back(std::make_pair(i->first.first, i->first.second));
        }

        return handles;
}

//=============================
// freeHandleMaps
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
void
TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::freeHandleMaps ()
{
    //free data
    /*
     * We iterate over all handles that are still in out predefined/user handle list.
     * We perform an mpi destroy on each remaining entry:
     * - If the destroy returns that
     *    the object still exist, then some module forgot to erase a persistent
     *    info.
     *
     * Important:
     * If we have such forgotten handles, we simply not free them, we must also
     * no call the "user destroy" function of HandleInfoBase! As handles may have
     * dependencies between each other, freeing some handle before another
     * may cause segfaults or assertions. So we just leave them to exist (which
     * only happens in an error case anyways).
     */
    ////User
    auto x_safe_UserHandles = xlock_safe_ptr(myUserHandles);
    typename HandleMap::const_iterator userIter = x_safe_UserHandles->begin();
    for (; userIter != x_safe_UserHandles->end(); userIter++)
    {
        if (!userIter->second) continue;

        if(!userIter->second->mpiDestroy())
        {
#ifdef MUST_DEBUG
            must::cout << "Warning: A module forgot to erase a persistent handle (it was also not freed by the application, thats why we saw this ...)." << std::endl;
#endif
        }
    }

    ////Predefined
    auto x_safe_Predefineds = xlock_safe_ptr(myPredefineds);
    typename PredefinedInfos::iterator preIter = x_safe_Predefineds->begin();
    for (; preIter != x_safe_Predefineds->end(); preIter++)
    {
        if (!preIter->second) continue;

        if(!preIter->second->mpiDestroy())
        {
#ifdef MUST_DEBUG
           must::cout << "Warning: some module failed to erase a persistent handle, you should breakpoint here and investigate." << std::endl;
#endif
        }
    }

    ////Remote
    auto x_safe_RemoteRes = xlock_safe_ptr(myRemoteRes);
    typename RemoteMap::iterator remoteIter = x_safe_RemoteRes->begin();
    for (; remoteIter != x_safe_RemoteRes->end(); remoteIter++)
    {
        RemoteResourceInfo info = remoteIter->second;
        if (!info.first) continue; //anything in there?
        if (info.second.first) continue; //If it had a handle we freed it in the user handle map already!

        if(!info.first->mpiDestroy())
        {
#ifdef MUST_DEBUG
            must::cout << "Warning: some module failed to erase a persistent handle on a remote resource, you should breakpoint here and investigate." << std::endl;
#endif
        }
    }

    ////Null
    if (myNullInfo)
        myNullInfo->mpiDestroy();
    myNullInfo = NULL;

    x_safe_Predefineds->clear();
    x_safe_UserHandles->clear();
    x_safe_RemoteRes->clear();
}

//=============================
// getHandleForInfo
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
bool TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::getHandleForInfo (
        int rank,
        FULL_INFO* info,
        HANDLE_TYPE *pOutHandle)
{
    //Is it NULL ?
    if (info == myNullInfo)
    {
      auto s_safe_NullValues = slock_safe_ptr(myNullValues);
      auto value = s_safe_NullValues->end();
      if (!s_safe_NullValues->empty() &&
          (value = s_safe_NullValues->find(rank)) != s_safe_NullValues->end()) {
        if (pOutHandle)
          *pOutHandle = value->second;
      } else {
        if (pOutHandle)
          *pOutHandle = myNullValue;
      }
        return true;
    }


    //Is it a user handle?
    //TODO we could speed this up by making HandleMap a two leveled map (rank->map<handle, info>)  O(p*n) -> O( ln(p) + n )
    // two leveled map is not that trivial in that case: crashes the caching + test for !=myUserHandles.end()!
    // O(p*n) is even worse in case of rank-based predefineds
    {
      auto s_safe_UserHandles = slock_safe_ptr(myUserHandles);
      typename HandleMap::const_iterator userIter;
      for (userIter = s_safe_UserHandles->begin();
           userIter != s_safe_UserHandles->end(); userIter++) {
        if (userIter->first.first != rank)
          continue;

        if (userIter->second == info) {
          if (pOutHandle)
            *pOutHandle = userIter->first.second;
          return true;
        }
      }
    }

        //Is it a (global-constant) predefined ?
    auto s_safe_Predefineds = slock_safe_ptr(myPredefineds);
    typename PredefinedInfos::const_iterator preIter;
    for (preIter = s_safe_Predefineds->begin();
         preIter != s_safe_Predefineds->end(); preIter++) {
      if (info == preIter->second) {
        if (pOutHandle)
          *pOutHandle = preIter->first;
        return true;
      }
    }

    return false;
}

//=============================
// notifyOfShutdown
//=============================
template <typename FULL_INFO, typename I_INFO, typename HANDLE_TYPE, typename PREDEFINED_ENUM, class SUPER, class INTERFACE>
void TrackBase<FULL_INFO, I_INFO, HANDLE_TYPE, PREDEFINED_ENUM, SUPER, INTERFACE>::notifyOfShutdown (void)
{
    HandleInfoBase::disableFreeForwardingAcross();
}

/*EOF*/
