/**
 * @file I_MpiTSanAnnotations.h
 * 	@see I_MpiTSanAnnotations.
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"
#include "I_ChannelId.h"
#include "MustTypes.h"

#include "StridedBlock.h"

#ifndef I_MPITSANANNOTATIONS_H
#define I_MPITSANANNOTATIONS_H

/**
 * Annotates MPI function calls using Thread Sanitizer.
 */
class I_MpiTSanAnnotations : public gti::I_Module
{
public:
	/**
         * Checks whether the buffers of a sendrecv-call overlap.
         * @param pId parallel context
         * @param lId location id of context.
         * @param sendbuf buffer for the sendpart.
         * @param sendcount number of repetitions for the sendpart.
         * @param sendtype datatype for the sendpart.
         * @param recvbuf buffer for the recvpart.
         * @param recvcount number of repetitions for the recvpart.
         * @param recvtype datatype for the recvpart.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN isSendRecvOverlapped (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType sendbuf,
                int sendcount,
                MustDatatypeType sendtype,
                MustAddressType recvbuf,
                int recvcount,
                MustDatatypeType recvtype
                ) = 0;

        /**
         * Checks whether the buffers of a sendrecv-kind-call overlap.
         * @param pId parallel context
         * @param lId location id of context.
         * @param sendbuf buffer for the sendpart.
         * @param senddispls displacements for the sendpart.
         * @param senddisplslen arraysize of displacements for the sendpart.
         * @param sendcounts numbers of repetitions for the sendpart.
         * @param sendcountslen arraysize of numbers of repetitions for the sendpart.
         * @param sendtypes datatypes for the sendpart.
         * @param sendtypeslen arraysize of datatypes for the sendpart.
         * @param recvbuf buffer for the recvpart.
         * @param recvdispls displacements for the recvpart.
         * @param recvdisplslen arraysize of displacements for the recvpart.
         * @param recvcounts numbers of repetitions for the recvpart.
         * @param recvcountslen arraysize of numbers of repetitions for the recvpart.
         * @param recvtypes datatypes for the recvpart.
         * @param recvtypeslen arraysize of datatypes for the recvpart.
         * @param hasRequest specify whether call has a request.
         * @param request for this communication call.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN isSendRecvOverlappedN (
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
                MustRequestType request
                ) = 0;
		
	/**
         * Checks for overlap of requests where isSend is set to true.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @param hasRequest specify whether call has a request.
         * @param request for this communication call.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN isendOverlapsRequests (
                MustParallelId pId,
                MustLocationId lId,
                MustDatatypeType datatype,
                MustAddressType buffer,
                int count,
                int hasRequest,
                MustRequestType request
                ) = 0;
		
	/**
         * Checks for overlaps of requests where isSend is set to true.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN sendOverlapsRequests (
                MustParallelId pId,
                MustLocationId lId,
                MustDatatypeType datatype,
                MustAddressType buffer,
                int count
		) = 0;
		
	/**
         * Checks whether a request overlaps memory regions spanned by open requests.
         * @param pId parallel context
         * @param lId location id of context.
         * @param buffer address of transfer buffer.
         * @param displs displacements for each block.
         * @param counts lengths of each block.
         * @param datatype to get references for.
         * @param commsize number of ranks.
         * @param hasRequest specify whether call has a request.
         * @param request for this communication call.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN sendOverlapcheckCounts (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType buffer,
                const int displs[],
                const int counts[],
                MustDatatypeType datatype,
                int commsize,
                int hasRequest,
                MustRequestType request
                ) = 0;
		
	/**
         * Checks whether a request overlaps memory regions spanned by open requests.
         * @param pId parallel context
         * @param lId location id of context.
         * @param buffer address of transfer buffer.
         * @param displs displacements for each block.
         * @param counts lengths of each block.
         * @param datatype to get references for.
         * @param commsize number of ranks.
         * @param hasRequest specify whether call has a request.
         * @param request for this communication call.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN sendOverlapcheckTypes (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType buffer,
                const int displs[],
                const int counts[],
                const MustDatatypeType datatypes[],
                int commsize,
                int hasRequest,
                MustRequestType request
                ) = 0;
		
	/**
         * Checks for overlap of requests where isSend is set to false.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @param hasRequest specify whether call has a request.
         * @param request for this communication call.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN irecvOverlapsRequests (
                MustParallelId pId,
                MustLocationId lId,
                MustDatatypeType datatype,
                MustAddressType buffer,
                int count,
                int hasRequest,
                MustRequestType request
                ) = 0;
		
	/**
         * Checks for overlap of requests where isSend is set to false.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN recvOverlapsRequests (
                MustParallelId pId,
                MustLocationId lId,
                MustDatatypeType datatype,
                MustAddressType buffer,
                int count
		) = 0;
	
	/**
         * Checks whether a request overlaps memory regions spanned by open requests.
         * @param pId parallel context
         * @param lId location id of context.
         * @param buffer address of transfer buffer.
         * @param displs displacements for each block.
         * @param counts lengths of each block.
         * @param datatype to get references for.
         * @param commsize number of ranks.
         * @param hasRequest specify whether call has a request.
         * @param request for this communication call.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
	virtual gti::GTI_ANALYSIS_RETURN recvOverlapcheckCounts (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType buffer,
                const int displs[],
                const int counts[],
                MustDatatypeType datatype,
                int commsize,
                int hasRequest,
                MustRequestType request
                ) = 0;
		
	/**
         * Checks whether a request overlaps memory regions spanned by open requests.
         * @param pId parallel context
         * @param lId location id of context.
         * @param buffer address of transfer buffer.
         * @param displs displacements for each block.
         * @param counts lengths of each block.
         * @param datatype to get references for.
         * @param commsize number of ranks.
         * @param hasRequest specify whether call has a request.
         * @param request for this communication call.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN recvOverlapcheckTypes (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType buffer,
                const int displs[],
                const int counts[],
                const MustDatatypeType datatypes[],
                int commsize,
                int hasRequest,
                MustRequestType request
                ) = 0;
	/**
	 * Convenience function for announcePRequest where isSend is set to true.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @param request handle of request
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN announcePSendRequest (
                MustParallelId pId,
                MustLocationId lId,
                MustDatatypeType datatype,
                MustAddressType buffer,
                int count,
                MustRequestType request
		) = 0;
		
	/**
         * Marks memory blocks as active for a new request where isSend is set to true.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @param request handle of request
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN announceSendRequest (
                MustParallelId pId,
                MustLocationId lId,
                MustDatatypeType datatype,
                MustAddressType buffer,
                int count,
                MustRequestType request
		) = 0;
		
	/**
         * Convenience function for announcePRequest where isSend is set to false.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @param request handle of request
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN announcePRecvRequest (
                MustParallelId pId,
                MustLocationId lId,
                MustDatatypeType datatype,
                MustAddressType buffer,
                int count,
                MustRequestType request
		) = 0;
		
	/**
         * Free prequest (Notification that a (persistent) Request is freed).
         * @param pId parallel context
         * @param lId location id of context.
         * @param request handle of request
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN freeRequest (
                MustParallelId pId,
                MustLocationId lId,
                MustRequestType request
		) = 0;
	
	/**
         * Marks memory blocks as active for a new request where isSend is set to false.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @param request handle of request
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN announceRecvRequest (
                MustParallelId pId,
                MustLocationId lId,
                MustDatatypeType datatype,
                MustAddressType buffer,
                int count,
                MustRequestType request
		) = 0;
	/**
         * Start persistent request.
         * @param pId parallel context
         * @param lId location id of context.
         * @param request handle of request
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN startPRequest (
                MustParallelId pId,
                MustLocationId lId,
                MustRequestType request
		) = 0;
		
	/**
         * Start array of persistent requests.
         * @param pId parallel context
         * @param lId location id of context.
         * @param requests handles of requests
         * @param count number of requests
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN startPRequestArray (
                MustParallelId pId,
                MustLocationId lId,
                MustRequestType* requests,
                int count
		) = 0;
		
	/**
         * Finish request (Notification that a communication associated with the
         * given request was completed).
         * @param pId parallel context
         * @param lId location id of context.
         * @param request handle of request
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN finishRequest (
                MustParallelId pId,
                MustLocationId lId,
                MustRequestType request
		) = 0;
		
	/**
         * Finish requests (Notification that for each entry in the array the
         * communication associated with the request was completed).
         * @param pId parallel context
         * @param lId location id of context.
         * @param requests handles of requests
         * @param count number of finished requests
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN finishRequests (
                MustParallelId pId,
                MustLocationId lId,
                MustRequestType* request,
                int count
		) = 0;
	
	/**
	 * Is called when hasRequest == true in order to associate map with the list of memory blocks.
	 * @param pId parallel context.
	 * @param lId location id of context.
	 * @param preparedList MustMemIntervalList.
	 * @param request MustRequest.
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
	virtual gti::GTI_ANALYSIS_RETURN makeBlocksActive (
		MustParallelId pId,
		MustLocationId lId,
		must::MustMemIntervalListType& preparedList,
		MustRequestType request
		) = 0;
	
	/**
	 * Is called when requests are announced or finished.
	 * @param pId parallel context.
	 * @param request MustRequest.
	 */
	virtual gti::GTI_ANALYSIS_RETURN makeBlocksInActive (
		MustParallelId pId,
		MustRequestType request
	) = 0;
	
	/**
	 * Is called when requests are announced or finished.
	 * @param rank process id.
	 * @param request MustRequest.
	 */
	virtual gti::GTI_ANALYSIS_RETURN makeBlocksInActive (
		int rank,
		MustRequestType request
	) = 0;
	
	/**
	 * @param rank process id.
	 * @param request MustRequest.
	 */
	virtual gti::GTI_ANALYSIS_RETURN freeRequestBlocks(
		int rank,
		MustRequestType request
        ) = 0;

  }; /* class I_MpiTSanAnnotations */

#endif /* I_MPITSANANNOTATIONS_H */