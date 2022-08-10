/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MsgLoggerStdOut.cpp
 *       @see MUST::MsgLoggerStdOut.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "MustEnums.h"
#include "MustDefines.h"

#include "PrefixedOstream.hpp"

#include "MsgLoggerStdOut.h"
#include <iostream>
#include <sstream>
#include <stddef.h>
#include <string>

using namespace must;

mGET_INSTANCE_FUNCTION(MsgLoggerStdOut)
mFREE_INSTANCE_FUNCTION(MsgLoggerStdOut)
mPNMPI_REGISTRATIONPOINT_FUNCTION(MsgLoggerStdOut)

//=============================
// Constructor
//=============================
MsgLoggerStdOut::MsgLoggerStdOut (const char* instanceName)
    : gti::ModuleBase<MsgLoggerStdOut, I_MessageLogger> (instanceName),
      myPIdModule (NULL),
      myLIdModule (NULL),
      myLoggedWarnError (false)
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
MsgLoggerStdOut::~MsgLoggerStdOut (void)
{
	if (myLIdModule)
		destroySubModuleInstance ((I_Module*) myLIdModule);
	myLIdModule = NULL;

	if (myPIdModule)
		destroySubModuleInstance ((I_Module*) myPIdModule);
	myPIdModule = NULL;
	if (!myLoggedWarnError) {
	  must::PrefixedOstream pcout{MUST_STDOUT_PREFIX_REPORT, std::cout};
	  pcout << "MUST detected no MPI usage errors nor any "
	    "suspicious behavior during this application run."
		<< std::endl;
	}
}

//=============================
// log
//=============================
GTI_ANALYSIS_RETURN MsgLoggerStdOut::log (
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
GTI_ANALYSIS_RETURN MsgLoggerStdOut::logStrided (
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

        std::stringstream out{};
	switch ((MustMessageType)msgType)
	{
	case MustErrorMessage:
		out << "Error";
		myLoggedWarnError = true;
		break;
	case MustWarningMessage:
		out << "Warning";
		myLoggedWarnError = true;
		break;
	case MustInformationMessage:
		out << "Information";
		break;
	default:
		out << "Unknown";
	}

	if (count > 0)
	{
	    out
	        << ": from: call " << myLIdModule->getInfoForId(pId,lId).callName << "@";

	    //CASE1: A single rank
	    if (count == 1)
	    {
#ifdef ELP_MODIFICATIONS
                ParallelInfo info = myPIdModule->getInfoForId(pId);
                out << startRank << "(" << info.threadid << ")";
#else
                out << startRank;
#endif
	    }
	    //CASE2: Continous ranks
	    else if (stride == 1)
	    {
	        out << startRank << "-" << startRank + (count-1);
	    }
	    //CASE3: Strided ranks
	    else
	    {
	        int last = startRank;
	        for (int x = 0; x < count; x++)
	        {
	            if (last != startRank)
	                out << ", ";

	            out << last;

	            last += stride;

	            if (x == 2 && count > 3)
	            {
	                out << ", ..., " << startRank + (count - 1) * stride;
	                break;
	            }
	        }
	    }

	    out << ": ";
	}
	else
	{
	    out << " global: ";
	}

	out << text;

	out
		<< " References";

//	if (count > 1)
	out << " (possibly of representative)";

    out << ": ";

	for (int i = 0; i < numReferences; i++)
	{
		out
				<< "Reference "<<(i+1)<<": "
				<< myLIdModule->toString(refPIds[i], refLIds[i])
				<< "@"
				<< myPIdModule->toString(refPIds[i])
				<< "; ";
	}

	out << std::endl;
	must::PrefixedOstream pcout{MUST_STDOUT_PREFIX_REPORT, std::cout};
	pcout << out.str();

	if ((MustMessageType)msgType == MustErrorMessage)
		pcout.flush();

	return GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
