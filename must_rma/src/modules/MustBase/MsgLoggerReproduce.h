/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MsgLoggerReproduce.h
 *       @see MUST::MsgLoggerReproduce.
 *
 *  @date 26.05.2014
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_MessageLogger.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"

#include <fstream>

#ifndef MSGLOGGERREPRODUCE_H
#define MSGLOGGERREPRODUCE_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_MessageLogger that prints into a
     * special file format that enables reproducing these messages in a second run.
     */
    class MsgLoggerReproduce : public gti::ModuleBase<MsgLoggerReproduce, I_MessageLogger>
    {
    public:
			/**
			 * Constructor.
			 * @param instanceName name of this module instance.
			 */
			MsgLoggerReproduce (const char* instanceName);

			/**
			 * Destructor.
			 */
			virtual ~MsgLoggerReproduce (void);

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

			/**
			 * Helper function that puts an entry into the actual log
			 */
		    bool logEntry (
		            bool isReference,
		            int rank,
		            std::string callName,
		            int repCount,
		            int msgType,
		            std::string text
		            );

    }; /*class MsgLoggerReproduce */
} /*namespace MUST*/

#endif /*MSGLOGGERREPRODUCE_H*/
