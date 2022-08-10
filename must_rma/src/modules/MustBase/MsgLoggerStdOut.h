/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MsgLoggerStdOut.h
 *       @see MUST::MsgLoggerStdOut.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_MessageLogger.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"

#ifndef MSGLOGGERSTDOUT_H
#define MSGLOGGERSTDOUT_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_MessageLogger that prints all logged events to
     * must::cout.
     */
    class MsgLoggerStdOut : public gti::ModuleBase<MsgLoggerStdOut, I_MessageLogger>
    {
    public:
			/**
			 * Constructor.
			 * @param instanceName name of this module instance.
			 */
			MsgLoggerStdOut (const char* instanceName);

			/**
			 * Destructor.
			 */
			virtual ~MsgLoggerStdOut (void);

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
    private:
            bool myLoggedWarnError;
    }; /*class MsgLoggerStdOut */
} /*namespace MUST*/

#endif /*MSGLOGGERSTDOUT_H*/
