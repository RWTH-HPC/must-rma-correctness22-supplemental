/**
 * @file MpiTSanAnnotations.h
 * 	@see MpiTSanAnnotations.
 */

#include "ModuleBase.h"
// #include "CompletionsTree.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"
#include "I_CreateMessage.h"
#include "I_ArgumentAnalysis.h"
#include "I_DatatypeTrack.h"
#include "I_RequestTrack.h"
#include "I_TSan.h"

#include "I_MpiTSanAnnotations.h"

#include <list>
#include <map>

#ifndef MPITSANANNOTATIONS_H
#define MPITSANANNOTATIONS_H

using namespace gti;

namespace must
{
  template <class T>
  class mustPidMap : public std::map<int, T > {};

  template <class T>
  class mustPidRequestMap : public std::map<int, std::map<MustRequestType, T > > {};
  
  struct LocationInfoFp {
    MustParallelId pId;
	MustLocationId lId;
	void* fp;
	LocationInfoFp()=default;
	LocationInfoFp(MustParallelId _pId, MustLocationId _lId, void* _fp=nullptr) : pId(_pId), lId(_lId), fp(_fp) {}
  };
  
  /**
   * Implementation of MPI function call annotation using TSan.
   */
  class MpiTSanAnnotations : public gti::ModuleBase<MpiTSanAnnotations, I_MpiTSanAnnotations>
  {
  public:
    /**
     * Constructor.
     * @param instanceName name of this module instance.
     */
    MpiTSanAnnotations (const char* instanceName);
    
    /**
     * Destructor.
     */
    virtual ~MpiTSanAnnotations (void);
    
    /**
     * @see I_MpiTSanAnnotations::isSendRecvOverlapped
     */
    GTI_ANALYSIS_RETURN isSendRecvOverlapped (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType sendbuf,
	int sendcount,
	MustDatatypeType sendtype,
	MustAddressType recvbuf,
	int recvcount,
	MustDatatypeType recvtype);
    
    /**
     * @see I_MpiTSanAnnotations::isSendRecvOverlappedN
     */
    GTI_ANALYSIS_RETURN isSendRecvOverlappedN (
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
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::isendOverlapsRequests
     */
    GTI_ANALYSIS_RETURN isendOverlapsRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	int hasRequest,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::sendOverlapsRequests
     */
    GTI_ANALYSIS_RETURN sendOverlapsRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count);
    
    /**
     * @see I_MpiTSanAnnotations::sendOverlapcheckCounts
     */
    GTI_ANALYSIS_RETURN sendOverlapcheckCounts (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	MustDatatypeType datatype,
	int commsize,
	int hasRequest,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::sendOverlapcheckTypes
     */
    GTI_ANALYSIS_RETURN sendOverlapcheckTypes (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	const MustDatatypeType datatypes[],
	int commsize,
	int hasRequest,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::irecvOverlapsRequests
     */
    GTI_ANALYSIS_RETURN irecvOverlapsRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	int hasRequest,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::recvOverlapsRequests
     */
    GTI_ANALYSIS_RETURN recvOverlapsRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count);
    
    /**
     * @see I_MpiTSanAnnotations::recvOverlapcheckCounts
     */
    GTI_ANALYSIS_RETURN recvOverlapcheckCounts (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	MustDatatypeType datatype,
	int commsize,
	int hasRequest,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::recvOverlapcheckTypes
     */
    GTI_ANALYSIS_RETURN recvOverlapcheckTypes (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	const MustDatatypeType datatypes[],
	int commsize,
	int hasRequest,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::announcePSendRequest
     */
    GTI_ANALYSIS_RETURN announcePSendRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::announceSendRequest
     */
    GTI_ANALYSIS_RETURN announceSendRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::announcePRecvRequest
     */
    GTI_ANALYSIS_RETURN announcePRecvRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::freeRequest
     */
    GTI_ANALYSIS_RETURN freeRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::announceRecvRequest
     */
    GTI_ANALYSIS_RETURN announceRecvRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustDatatypeType datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::startPRequest
     */
    GTI_ANALYSIS_RETURN startPRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::startPRequestArray
     */
    GTI_ANALYSIS_RETURN startPRequestArray (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType* requests,
	int count);
    /**
     * @see I_MpiTSanAnnotations::finishRequest
     */
    GTI_ANALYSIS_RETURN finishRequest (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType request);
    
    /**
     * @see I_MpiTSanAnnotations::finishRequests
     */
    GTI_ANALYSIS_RETURN finishRequests (
	MustParallelId pId,
	MustLocationId lId,
	MustRequestType* request,
	int count);
    
    /**
     * @see I_MpiTSanAnnotations::annotateReadFromMemIntervalList
     */
    void annotateReadFromMemIntervalList (
	MustParallelId pId,
	MustLocationId lId,
	MustMemIntervalListType& iList);
    
    /**
     * @see I_MpiTSanAnnotations::annotateFromMemIntervalList
     */
    void annotateFromMemIntervalList (
	MustParallelId pId, 
	MustLocationId lId, 
	MustMemIntervalListType& iList,
	void* fp);
    
    /**
     * @see I_MpiTSanAnnotations::annotateWriteFromMemIntervalList
     */
    void annotateWriteFromMemIntervalList (
	MustParallelId pId,
	MustLocationId lId,
	MustMemIntervalListType& iList);
    
    /**
     * @see I_MpiTSanAnnotations::makeBlocksActive
     */
    GTI_ANALYSIS_RETURN makeBlocksActive (
	MustParallelId pId,
	MustLocationId lId,
	must::MustMemIntervalListType& preparedList,
	MustRequestType request
	);
    
    /**
     * @see I_MpiTSanAnnotations::makeBlocksInActive
     */
    GTI_ANALYSIS_RETURN makeBlocksInActive (
	MustParallelId pId,
	MustRequestType request
	);
    
    /**
     * @see I_MpiTSanAnnotations::makeBlocksInActive
     */
    GTI_ANALYSIS_RETURN makeBlocksInActive (
	int rank,
	MustRequestType request
	);
    
    /**
     * @see I_MpiTSanAnnotations::freeRequestBlocks
     */
    GTI_ANALYSIS_RETURN freeRequestBlocks(
        int rank,
        MustRequestType request
        );
	
  protected:
    MustMemIntervalListType calcIntervalList (
	I_Datatype * datatype,
	MustAddressType buffer,
	int count,
	MustRequestType request,
	bool isSend);

    
    I_ParallelIdAnalysis* myPIdMod;
    I_LocationAnalysis* myLIdMod;
    I_CreateMessage* myLogger;
    I_ArgumentAnalysis* myArgMod;
    I_DatatypeTrack* myDatMod;
    I_RequestTrack* myReqMod;
    I_TSan* myTSanMod;
    
    /*mustPidMap<MustMemIntervalListType> activeBlocks;
    mustPidRequestMap<std::list<MustMemIntervalListType::iterator>> activeRequestsBlocklists;*/

    mustPidRequestMap<MustMemIntervalListType> requestBlocklist;
    mustPidRequestMap< MustMemIntervalListType > preparedBlocklists;
    mustPidRequestMap<LocationInfoFp> requestLocation;
	mustPidRequestMap<void*> fpList;
    
  }; /* class MpiTSanAnnotations */
} /* namespace must */

#endif /* MPITSANANNOTATIONS_H */
