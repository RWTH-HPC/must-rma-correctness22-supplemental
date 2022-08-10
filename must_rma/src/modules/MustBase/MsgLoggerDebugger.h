/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MsgLoggerDebugger.h
 *       @see MUST::MsgLoggerDebugger.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_MessageLogger.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"

#ifndef MSGLOGGERDEBUGGER_H
#define MSGLOGGERDEBUGGER_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_MessageLogger that prints all logged events to
     * must::cout.
     */
    class MsgLoggerDebugger : public gti::ModuleBase<MsgLoggerDebugger, I_MessageLogger>
    {
    public:
			/**
			 * Constructor.
			 * @param instanceName name of this module instance.
			 */
			MsgLoggerDebugger (const char* instanceName);

			/**
			 * Destructor.
			 */
			virtual ~MsgLoggerDebugger (void);

			/**
			 * @see I_MessageLogger::log.
			 */
			GTI_ANALYSIS_RETURN log (
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
					uint64_t* refLIds
			);

			/**
			 * @see I_MessageLogger::logStrided.
			 */
			GTI_ANALYSIS_RETURN logStrided (
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
			        uint64_t* refLIds
			);

    protected:
			I_ParallelIdAnalysis *myPIdModule;
			I_LocationAnalysis *myLIdModule;
    }; /*class MsgLoggerDebugger */
} /*namespace MUST*/

#endif /*MSGLOGGERDEBUGGER_H*/
