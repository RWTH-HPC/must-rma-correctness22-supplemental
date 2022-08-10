/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OpSplitter.cpp
 *       @see MUST::OpSplitter.
 *
 *  @date 05.04.2011
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "OpSplitter.h"

#include "DeadlockApi.h"

using namespace must;

mGET_INSTANCE_FUNCTION(OpSplitter)
mFREE_INSTANCE_FUNCTION(OpSplitter)
mPNMPI_REGISTRATIONPOINT_FUNCTION(OpSplitter)

//=============================
// Constructor
//=============================
OpSplitter::OpSplitter (const char* instanceName)
    : gti::ModuleBase<OpSplitter, I_OpSplitter> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

	for (std::vector<I_Module*>::size_type i = 0; i < subModInstances.size(); i++)
    		destroySubModuleInstance (subModInstances[i]);

    //Initialize module data
	/*Nothing to do*/
}

//=============================
// Destructor
//=============================
OpSplitter::~OpSplitter ()
{
	/*Nothing to do*/
}

//=============================
// splitSendRecv
//=============================
GTI_ANALYSIS_RETURN OpSplitter::splitSendRecv (
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        int sendtag,
        MustDatatypeType sendtype,
        int sendcount,
        int source,
        int recvtag,
        MustCommType recvtype,
        int recvcount,
        MustCommType comm)
{
	//Create the send event
	splitSendP fPS;
	if (getWrapperFunction ("splitSend", (GTI_Fct_t*)&fPS) == GTI_SUCCESS)
	{
		(*fPS) (
				pId,
				lId,
				dest,
				sendtag,
				comm,
				sendtype,
				sendcount
				);
	}

	//Create the receive event
	splitRecvP fPR;
	if (getWrapperFunction ("splitRecv", (GTI_Fct_t*)&fPR) == GTI_SUCCESS)
	{
		(*fPR) (
				pId,
				lId,
				source,
				recvtag,
				comm,
				recvtype,
				recvcount
		);
	}

	return GTI_ANALYSIS_SUCCESS;
}

//=============================
// splitStartall
//=============================
GTI_ANALYSIS_RETURN OpSplitter::splitStartall (
		MustParallelId pId,
		MustLocationId lId,
		int count,
		MustRequestType *requests)
{
	splitStartP fP;
	if (getWrapperFunction ("splitStart", (GTI_Fct_t*)&fP) == GTI_SUCCESS)
	{
		for (int i = 0; i < count; i++)
		{
			(*fP) (
				pId,
				lId,
				requests[i]
				);
		}
	}

	return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
