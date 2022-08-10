/**
 * @file MpiTSanAnnotations.cpp
 * 	@see MpiTSanAnnotations.
 */

#include "GtiMacros.h"
#include "MpiTSanAnnotations.h"
#include "MustEnums.h"
#include "MustDefines.h"
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>

#include <sstream>
#include <fstream>

using namespace must;

mGET_INSTANCE_FUNCTION(MpiTSanAnnotations)
mFREE_INSTANCE_FUNCTION(MpiTSanAnnotations)
mPNMPI_REGISTRATIONPOINT_FUNCTION(MpiTSanAnnotations)

//=============================
// Constructor
//=============================
MpiTSanAnnotations::MpiTSanAnnotations (const char* instanceName)
    : gti::ModuleBase<MpiTSanAnnotations, I_MpiTSanAnnotations> (instanceName),
      preparedBlocklists (),
      requestLocation (),
      requestBlocklist ()
{
    // create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();
    
    // handle sub modules
#define NUM_SUBMODULES 7
    if (subModInstances.size() < NUM_SUBMODULES)
    {
	std::cerr << "Module does not have enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
	assert (0);
    }
    if (subModInstances.size() > NUM_SUBMODULES)
    {
	for (std::vector<I_Module*>::size_type i = NUM_SUBMODULES; i < subModInstances.size(); ++i)
	{
	    destroySubModuleInstance (subModInstances[i]);
	}
    }
    
    myPIdMod = (I_ParallelIdAnalysis*) subModInstances[0];
    myLIdMod = (I_LocationAnalysis*) subModInstances[1];
    myLogger = (I_CreateMessage*) subModInstances[2];
    myArgMod = (I_ArgumentAnalysis*) subModInstances[3];
    myDatMod = (I_DatatypeTrack*) subModInstances[4];
    myReqMod = (I_RequestTrack*) subModInstances[5];
    myTSanMod = (I_TSan*) subModInstances[6];
}

//=============================
// Destructor
//=============================
MpiTSanAnnotations::~MpiTSanAnnotations (void)
{
    if (myPIdMod)
	destroySubModuleInstance ((I_Module*) myPIdMod);
    myPIdMod = NULL;

    if (myLIdMod)
	destroySubModuleInstance ((I_Module*) myLIdMod);
    myLIdMod = NULL;
    
    if (myLogger)
	destroySubModuleInstance ((I_Module*) myLogger);
    myLogger = NULL;
    
    if (myArgMod)
	destroySubModuleInstance ((I_Module*) myArgMod);
    myArgMod = NULL;
    
    if (myDatMod)
	destroySubModuleInstance ((I_Module*) myDatMod);
    myDatMod = NULL;
    
    if (myReqMod)
	destroySubModuleInstance ((I_Module*) myReqMod);
    myReqMod = NULL;
    
    if (myTSanMod)
	destroySubModuleInstance ((I_Module*) myTSanMod);
    myTSanMod = NULL;
}

//=============================
// annotateReadFromMemIntervalList
//=============================
/*
void MpiTSanAnnotations::annotateReadFromMemIntervalList(
	MustParallelId pId, 
	MustLocationId lId, 
	MustMemIntervalListType& iList)
{
  for(auto stridedBlock : iList)
  {
    if (stridedBlock.stride == 0)
      myTSanMod->annotateMemoryRead(pId, lId, stridedBlock.first, stridedBlock.blocksize);
    else
      for(MustAddressType addr = stridedBlock.first; addr < stridedBlock.second; addr += stridedBlock.stride)
      {
        myTSanMod->annotateMemoryRead(pId, lId, addr, stridedBlock.blocksize);
      }
  }
}
*/


//The const kPCInc must be in sync with StackTrace::GetPreviousInstructionPc
#if defined(__powerpc64__) || defined(__arm__) || defined(__aarch64__)
// PCs are always 4 byte aligned.
const int kPCInc = 4;
#elif defined(__sparc__) || defined(__mips__)
const int kPCInc = 8;
#else
const int kPCInc = 1;
#endif

//=============================
// annotateFromMemIntervalList.
//=============================
#define TSAN_MARK(a) ((a + 1) | 1ULL<<60)
//#define TSAN_MARK(a) (a)
void MpiTSanAnnotations::annotateFromMemIntervalList(
	MustParallelId pId, 
	MustLocationId lId, 
	MustMemIntervalListType& iList,
    void* fp=nullptr)
{
  void *pc;
  PNMPI_Service_GetReturnAddress(&pc);
  if (!fp) {
    PNMPI_Service_GetFunctionAddress(&fp);
  }

  // Function entry
  myTSanMod->annotateFuncEntry((uptr)pc);
  for(auto stridedBlock : iList)
  {
#ifdef MUST_DEBUG
    std::cout << "Annotation: isSend=" << stridedBlock.isSend << ", first=" << std::hex << stridedBlock.first
              << ", second=" << std::hex << stridedBlock.second << std::endl;
#endif
    if(stridedBlock.isSend)
    {
      if (stridedBlock.stride == 0)
      {
	myTSanMod->annotateMemoryReadPC(pId, lId, stridedBlock.first, stridedBlock.blocksize, ((uptr)fp)+kPCInc);
      }
      else
      {
	for(MustAddressType addr = stridedBlock.first; addr < stridedBlock.second; addr += stridedBlock.stride)
	{
	  myTSanMod->annotateMemoryReadPC(pId, lId, addr, stridedBlock.blocksize, ((uptr)fp)+kPCInc);
	}
      }
    }
    else
    {
      if (stridedBlock.stride == 0)
      {
	myTSanMod->annotateMemoryWritePC(pId, lId, stridedBlock.first, stridedBlock.blocksize, ((uptr)fp)+kPCInc);
      }
      else
      {
	for(MustAddressType addr = stridedBlock.first; addr < stridedBlock.second; addr += stridedBlock.stride)
	{
	  myTSanMod->annotateMemoryWritePC(pId, lId, addr, stridedBlock.blocksize, ((uptr)fp)+kPCInc);
	}
      }
    }
  }

    // Function exit
  myTSanMod->annotateFuncExit();

}

//=============================
// annotateWriteFromMemIntervalList
//=============================
/*
void MpiTSanAnnotations::annotateWriteFromMemIntervalList(
	MustParallelId pId, 
	MustLocationId lId, 
	MustMemIntervalListType& iList)
{
  for(auto stridedBlock : iList)
  {
    if (stridedBlock.stride == 0)
      myTSanMod->annotateMemoryWrite(pId, lId, stridedBlock.first, stridedBlock.blocksize);
    else
      for(MustAddressType addr = stridedBlock.first; addr < stridedBlock.second; addr += stridedBlock.stride)
      {
	myTSanMod->annotateMemoryWrite(pId, lId, addr, stridedBlock.blocksize);
      }
  }
}
*/

//=============================
// makeBlocksActive
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::makeBlocksActive (
	MustParallelId pId,
	MustLocationId lId,
	MustMemIntervalListType& preparedList,
	MustRequestType request
	)
{
    void *fp;
    PNMPI_Service_GetFunctionAddress(&fp);

    int rank = myPIdMod->getInfoForId(pId).rank;;
    requestLocation[rank][request] = LocationInfoFp(pId, lId, fp);
    auto list = requestBlocklist[rank].find(request);
    if (list == requestBlocklist[rank].end())
        requestBlocklist[rank][request] = preparedList;
    else
        list->second.insert(preparedList.begin(), preparedList.end());
    myTSanMod->annotateInitTLC(pId, lId, &(requestBlocklist[rank][request]));

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// makeBlocksInActive  TODO -> annotateAndFreeRequest
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::makeBlocksInActive(
        MustParallelId pId,
        MustRequestType request
        )
{
    int rank = myPIdMod->getInfoForId(pId).rank;
    return makeBlocksInActive(rank, request);
}

GTI_ANALYSIS_RETURN MpiTSanAnnotations::makeBlocksInActive(
        int rank,
        MustRequestType request
        )
{

    static int temp;
    myTSanMod->annotateHappensBefore(0,0,&temp); // store initial state
    
    auto info = requestLocation[rank][request];
    myTSanMod->annotateStartTLC(0, 0, &(requestBlocklist[rank][request])); // load TLC state
    annotateFromMemIntervalList(info.pId, info.lId, requestBlocklist[rank][request], info.fp);
    myTSanMod->annotateHappensAfter(0,0,&temp); // restore initial state

    // after deleting the blocks from activeBlocks, delete the list of bookmarks
    requestLocation[rank].erase(request);
    requestBlocklist[rank].erase(request);
    return GTI_ANALYSIS_SUCCESS;
}

GTI_ANALYSIS_RETURN MpiTSanAnnotations::freeRequestBlocks(
        int rank,
        MustRequestType request
        )
{
    // after deleting the blocks from activeBlocks, delete the list of bookmarks
    requestLocation[rank].erase(request);
    requestBlocklist.erase(request);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// isSendRecvOverlapped
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::isSendRecvOverlapped (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType sendbuf,
	int sendcount,
	MustDatatypeType sendtype,
	MustAddressType recvbuf,
	int recvcount,
	MustDatatypeType recvtype)
{
    return MpiTSanAnnotations::isSendRecvOverlappedN(pId, lId, sendbuf, NULL, 0, &sendcount, 1, &sendtype, 1, recvbuf, NULL, 0, &recvcount, 1, &recvtype, 1, false, 0);
}

//=============================
// isSendRecvOverlappedN
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::isSendRecvOverlappedN (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType sendbuf,
	const int *senddispls,
	int senddisplslen,
	const int *sendcounts,
	int sendcountslen,
	const MustDatatypeType *sendtypes,
	int sendtypeslen,
	MustAddressType recvbuf,
	const int *recvdispls,
	int recvdisplslen,
	const int *recvcounts,
	int recvcountslen,
	const MustDatatypeType *recvtypes,
	int recvtypeslen,
	int hasRequest,
	MustRequestType request)
{
    if (sendcountslen < 1 || sendtypeslen < 1 || recvcountslen < 1 || recvtypeslen < 1
	|| senddisplslen < 0 || recvdisplslen < 0 || (senddisplslen > 0 && senddispls == NULL)
	|| (recvdisplslen > 0 && recvdispls == NULL))
    {
      std::cout << "Implementation error: incorrect call of MpiTSanAnnotations:isSendRecvOverlappedN!" << std::endl;
      return GTI_ANALYSIS_SUCCESS;
    }
    if (!hasRequest)
    {
	request = 0;
    }
    
    MustMemIntervalListType iList, tList;
    int displacement = 0;
    int count = sendcounts[0];
    MustDatatypeType type = sendtypes[0];
    
    I_Datatype* typeinfo = myDatMod->getDatatype(pId, type);
    if(typeinfo == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    MustAddressType extent = typeinfo->getExtent();
    
    for (int i = 0; i < sendcountslen; ++i)
    {
      if (senddisplslen > 1)
      {
	    displacement = senddispls[i];
      }
      if (sendcountslen > 1)
      {
	    count = sendcounts[i];
      }
      if (sendtypeslen > 1)
      {
	  type = sendtypes[i];
	  typeinfo = myDatMod->getDatatype(pId, type);
	  if (typeinfo == NULL)
	  {
	      return GTI_ANALYSIS_SUCCESS;
	  }
      }
      else 
      {
	  displacement *= extent;
      }
      tList = calcIntervalList (typeinfo, sendbuf + displacement, count, (MustRequestType) 0, true);
      iList.insert (tList.begin(), tList.end());
    }
    if (hasRequest)
    {
	makeBlocksActive(pId, lId, iList, request);
    } else
    {
	annotateFromMemIntervalList(pId, lId, iList);
    }
    
    for (int i = 0; i < recvcountslen; ++i)
    {
      if (recvdisplslen > 1)
      {
	    displacement = recvdispls[i];
      }
      if (recvcountslen > 1)
      {
	    count = recvcounts[i];
      }
      if (recvtypeslen > 1)
      {
	  type = recvtypes[i];
	  typeinfo = myDatMod->getDatatype(pId, type);
	  if (typeinfo == NULL)
	  {
	      return GTI_ANALYSIS_SUCCESS;
	  }
      }
      else 
      {
	  displacement *= extent;
      }
      tList = calcIntervalList (typeinfo, recvbuf + displacement, count, (MustRequestType) 0, false);
      iList.insert (tList.begin(), tList.end());
    }
    if (hasRequest)
    {
	makeBlocksActive(pId, lId, iList, request);
    } else
    {
	annotateFromMemIntervalList(pId, lId, iList);
    }
    
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// isendOverlapsRequests
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::isendOverlapsRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	int hasRequest,
	MustRequestType request)
{
    if(buffer == MUST_IN_PLACE) 
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if(buffer == MUST_BOTTOM) 
    {
        buffer = 0;
    }
    
    I_Datatype * typeinfo = myDatMod->getDatatype(pId, datatype);
    if (!hasRequest) 
    {
	request = 0;
    }
    if (typeinfo == NULL) 
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    
    MustMemIntervalListType iList;
    iList = calcIntervalList(typeinfo, buffer, count, request, true);
    if (hasRequest)
    {
	makeBlocksActive(pId, lId, iList, request);
    } else
    {
	annotateFromMemIntervalList(pId, lId, iList);
    }
    
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// sendOverlapsRequests
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::sendOverlapsRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count)
{
    if(buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if(buffer == MUST_BOTTOM)
    {
        buffer = 0;
    }
    I_Datatype * typeinfo = myDatMod->getDatatype(pId, datatype);
    if (typeinfo == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }

    MustMemIntervalListType iList = calcIntervalList(typeinfo, buffer, count, (MustRequestType) 0, true);
    annotateFromMemIntervalList(pId, lId, iList);
    
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// sendOverlapcheckCounts
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::sendOverlapcheckCounts (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	MustDatatypeType datatype,
	int commsize,
	int hasRequest,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    
    MustMemIntervalListType iList, tList;
    I_Datatype* typeinfo = myDatMod->getDatatype(pId, datatype);
    
    if (typeinfo == NULL || displs == NULL || counts == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    MustAddressType extent = typeinfo->getExtent();
    
    if (!hasRequest)
    {
	request = 0;
    }
    
    for (int i = 0; i < commsize; ++i)
    {
	tList = calcIntervalList(typeinfo, buffer+displs[i]*extent, counts[i], request, true);
	iList.insert(tList.begin(), tList.end());
    }
    
    if (hasRequest)
    {
	makeBlocksActive(pId, lId, iList, request);
    }
    else
    {
	annotateFromMemIntervalList(pId, lId, iList);
    }
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// sendOverlapcheckTypes
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::sendOverlapcheckTypes (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	const MustDatatypeType datatypes[],
	int commsize,
	int hasRequest,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    
    MustMemIntervalListType iList, tList;
    I_Datatype* typeinfo;
    
    if (displs == NULL || counts == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    
    if (!hasRequest)
    {
      request = 0;
    }
    
    for (int i = 0; i < commsize; ++i)
    {
	typeinfo = myDatMod->getDatatype(pId, datatypes[i]);
	if (typeinfo == NULL)
	{
	    return GTI_ANALYSIS_SUCCESS;
	}
	tList = calcIntervalList(typeinfo, buffer+displs[i], counts[i], request, true);
	iList.insert(tList.begin(), tList.end());
    }
    
    if(hasRequest) 
    {
	makeBlocksActive(pId, lId, iList, request);
    }
    else
    {
	annotateFromMemIntervalList(pId, lId, iList);
    }
    
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// irecvOverlapsRequests
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::irecvOverlapsRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	int hasRequest,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if(buffer == MUST_BOTTOM)
    {
	 buffer = 0;
    }
    
    I_Datatype * typeinfo = myDatMod->getDatatype(pId, datatype);
    if (typeinfo == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    
    MustMemIntervalListType iList = calcIntervalList(typeinfo, buffer, count, request, false);
    if (hasRequest)
    {
        makeBlocksActive(pId, lId, iList, request);
    }
    else
    {
	annotateFromMemIntervalList(pId, lId, iList);
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvOverlapsRequests
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::recvOverlapsRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count)
{
    if(buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if(buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    I_Datatype* typeinfo = myDatMod->getDatatype(pId, datatype);
    if(typeinfo == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    MustMemIntervalListType iList = calcIntervalList(typeinfo, buffer, count, (MustRequestType) 0, false);
    annotateFromMemIntervalList(pId, lId, iList);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvOverlapcheckCounts
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::recvOverlapcheckCounts (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	MustDatatypeType datatype,
	int commsize,
	int hasRequest,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    MustMemIntervalListType iList, tList;
    I_Datatype* typeinfo = myDatMod->getDatatype(pId, datatype);
    if (typeinfo == NULL || displs == NULL || counts == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    MustAddressType extent = typeinfo->getExtent();
    if (!hasRequest)
    {
      request = 0;
    }
    
    for (int i = 0; i < commsize; ++i)
    {
	tList = calcIntervalList(typeinfo, buffer + displs[i]*extent, counts[i], request, false);
	iList.insert(tList.begin(), tList.end());
    }
    
    if (hasRequest)
    {
      makeBlocksActive(pId, lId, iList, request);
    }
    else
    {
	annotateFromMemIntervalList(pId, lId, iList);
    }
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvOverlapcheckTypes
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::recvOverlapcheckTypes (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	const MustDatatypeType datatypes[],
	int commsize,
	int hasRequest,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    MustMemIntervalListType iList, tList;
    I_Datatype* typeinfo;
    
    if (displs == NULL || counts == NULL || datatypes == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (!hasRequest)
    {
      request = 0;
    }
    
    for (int i = 0; i < commsize; ++i)
    {
	typeinfo = myDatMod->getDatatype(pId, datatypes[i]);
	if (typeinfo == NULL)
	{
	    return GTI_ANALYSIS_SUCCESS;
	}
	tList = calcIntervalList(typeinfo, buffer+displs[i], counts[i], request, false);
	iList.insert(tList.begin(), tList.end());
    }
    
    if (hasRequest)
    {
	makeBlocksActive(pId, lId, iList, request);
    }
    else
    {
	annotateFromMemIntervalList(pId, lId, iList);
    }
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// announcePSendRequest
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::announcePSendRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    
    I_Datatype* typeinfo = myDatMod->getDatatype(pId, datatype);
    if (typeinfo == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    preparedBlocklists[pId][request] = calcIntervalList(typeinfo, buffer, count, request, true);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// announceSendRequest
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::announceSendRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    I_Datatype* typeinfo = myDatMod->getDatatype(pId, datatype);
    if (typeinfo == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    MustMemIntervalListType iList = calcIntervalList(typeinfo, buffer, count, request, true);
    makeBlocksActive(pId, lId, iList, request);
    
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// announcePRecvRequest
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::announcePRecvRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    
    I_Datatype* typeinfo = myDatMod->getDatatype(pId, datatype);
    if (typeinfo == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    preparedBlocklists[pId][request] = calcIntervalList(typeinfo, buffer, count, request, false);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// freeRequest
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::freeRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType request)
{
    I_Request* rInfo = myReqMod->getRequest(pId, request);
    if (rInfo->isPersistent())
    {
        preparedBlocklists[pId].erase(request);
    }
    freeRequestBlocks(myPIdMod->getInfoForId(pId).rank, request);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// announceRecvRequest
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::announceRecvRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request)
{
    if (buffer == MUST_IN_PLACE)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    if (buffer == MUST_BOTTOM)
    {
	buffer = 0;
    }
    I_Datatype* typeinfo = myDatMod->getDatatype(pId, datatype);
    if (typeinfo == NULL)
    {
	return GTI_ANALYSIS_SUCCESS;
    }
    MustMemIntervalListType iList = calcIntervalList(typeinfo, buffer, count, request, false);
    makeBlocksActive(pId, lId, iList, request);
    
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// startPRequest
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::startPRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType request)
{
    mustPidRequestMap< MustMemIntervalListType >::iterator pIdMapPos = preparedBlocklists.find (myPIdMod->getInfoForId(pId).rank);
    if (pIdMapPos == preparedBlocklists.end ())
    {
	    return GTI_ANALYSIS_SUCCESS;
    }
	    
    std::map<MustRequestType, MustMemIntervalListType>::iterator reqMapPos = pIdMapPos->second.find (request);
    
    if (reqMapPos ==  pIdMapPos->second.end())
    {
	return GTI_ANALYSIS_SUCCESS;
    }

    makeBlocksActive(pId, lId, reqMapPos->second, request);
    
    // the same as: makeBlocksActive(pId, lId, preparedBlocklists[rank][request], request);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// startPRequestArray
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::startPRequestArray (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType* requests,
	int count)
{
    for (int i = 0; i < count; ++i)
    {
	startPRequest(pId, lId, requests[i]);
    }
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// finishRequest
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::finishRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType request)
{
    makeBlocksInActive(pId, request);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// finishRequests
//=============================
GTI_ANALYSIS_RETURN MpiTSanAnnotations::finishRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType* request,
	int count)
{
    for (int i = 0; i < count; ++i)
    {
	makeBlocksInActive(pId, request[i]);
    }
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// calcIntervalList
//=============================
MustMemIntervalListType MpiTSanAnnotations::calcIntervalList (
	I_Datatype * typeinfo,
	MustAddressType buffer,
	int count,
	MustRequestType request,
	bool isSend)
{
    static MustMemIntervalListType ret;
    static I_Datatype * lastinfo = NULL;
    static MustAddressType lastbuffer = 0;
    static int lastcount = 0;
    static MustRequestType lastrequest = 0;
    if(typeinfo == lastinfo && lastcount == count && lastbuffer == buffer && request == lastrequest)
    {
        return ret;
    }
    ret.clear();
    lastinfo = typeinfo;
    lastcount = count;
    lastbuffer = buffer;
    lastrequest = request;
    { // get the blocklist for the datatype and repeate as requested (count)
        BlockInfo& blockInfo = typeinfo->getBlockInfo();
        MustAddressType extent = typeinfo->getExtent();
        MustAddressType size = typeinfo->getSize();
        ret = buildMemIntervallist(blockInfo, extent, size, buffer, request, isSend, typeinfo, count, buffer);
    }
    
    return ret;
}