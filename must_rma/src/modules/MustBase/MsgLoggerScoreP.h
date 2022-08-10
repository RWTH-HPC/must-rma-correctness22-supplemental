/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MsgLoggerScoreP.h
 *       @see MUST::MsgLoggerScoreP.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_MessageLogger.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"

#include <fstream>

#ifndef MSGLOGGERSCOREP_H
#define MSGLOGGERSCOREP_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_MessageLogger that prints into a
     * semicolumn seperated file for ScoreP.
     */
    class MsgLoggerScoreP : public gti::ModuleBase<MsgLoggerScoreP, I_MessageLogger>
    {
    public:
			/**
			 * Constructor.
			 * @param instanceName name of this module instance.
			 */
			MsgLoggerScoreP (const char* instanceName);

			/**
			 * Destructor.
			 */
			virtual ~MsgLoggerScoreP (void);

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

			std::ofstream myLog;
    }; /*class MsgLoggerScoreP */
} /*namespace MUST*/

#endif /*MSGLOGGERSCOREP_H*/
