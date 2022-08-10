/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MsgLoggerDebugger.cpp
 *       @see MUST::MsgLoggerDebugger.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "PrefixedOstream.hpp"

#include "MsgLoggerDebugger.h"

using namespace must;

mGET_INSTANCE_FUNCTION(MsgLoggerDebugger)
mFREE_INSTANCE_FUNCTION(MsgLoggerDebugger)
mPNMPI_REGISTRATIONPOINT_FUNCTION(MsgLoggerDebugger)

//=============================
// Constructor
//=============================
MsgLoggerDebugger::MsgLoggerDebugger (const char* instanceName)
    : gti::ModuleBase<MsgLoggerDebugger, I_MessageLogger> (instanceName),
      myPIdModule (NULL),
      myLIdModule (NULL)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //save sub modules
    myLIdModule = (I_LocationAnalysis*) subModInstances[0];
    myPIdModule = (I_ParallelIdAnalysis*) subModInstances[1];
}

//=============================
// Destructor
//=============================
MsgLoggerDebugger::~MsgLoggerDebugger (void)
{
	if (myLIdModule)
		destroySubModuleInstance ((I_Module*) myLIdModule);
	myLIdModule = NULL;

	if (myPIdModule)
		destroySubModuleInstance ((I_Module*) myPIdModule);
	myPIdModule = NULL;
}

//=============================
// log
//=============================
GTI_ANALYSIS_RETURN MsgLoggerDebugger::log (
          int msgId,
          int hasLocation,
          uint64_t pId,
          uint64_t lId,
          size_t fileId,
          int msgType,
          char *text,
          int textLen,
          int numReferences,
          uint64_t* refPIds,
          uint64_t* refLIds)
{
    if (!hasLocation)
        return logStrided (msgId, pId, lId, fileId, 0, 0, 0, msgType, text, textLen, numReferences, refPIds, refLIds);

    return logStrided (msgId, pId, lId, fileId, myPIdModule->getInfoForId(pId).rank, 1, 1, msgType, text, textLen, numReferences, refPIds, refLIds);
}

//=============================
// logStrided
//=============================
GTI_ANALYSIS_RETURN MsgLoggerDebugger::logStrided (
					int msgId,
					uint64_t pId,
					uint64_t lId,
					size_t fileId,
					int startRank,
					int stride,
					int count,
					int msgType,
					char *text,
					int textLen,
					int numReferences,
					uint64_t* refPIds,
					uint64_t* refLIds)
{
    switch ((MustMessageType)msgType)
    {
    case MustErrorMessage:
        break;
    default:
        return GTI_ANALYSIS_SUCCESS;
    }

    must::cout
        << "Stopping process " << getpid() << ": " << std::endl
        << "gdb -p " << getpid()
        << std::endl;

    must::cout.flush();

    bool loop=true;
    while (loop)
        sleep(1);

    return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
