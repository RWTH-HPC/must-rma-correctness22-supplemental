/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_MessageReproducer.h
 *       @see I_MessageReproducer.
 *
 *  @date 26.05.2014
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "I_CreateMessage.h"
#include "I_ChannelId.h"

#include "BaseIds.h"

#ifndef I_MESSAGEREPRODUCER_H
#define I_MESSAGEREPRODUCER_H

/**
 * Interface for replaying a log of MUST messages during an application execution.
 * Targets use-cases such as hitting a breakpoint when the application reaches an MPI call that caused an error/warning in a previous
 * execution.
 *
 * Dependencies:
 * - I_ParallelIdAnalysis
 * - I_LocationAnalysis
 * - I_CreateMessage
 */
class I_MessageReproducer : public gti::I_Module
{
public:
	/**
	 * Checks whether the given pId and lId matches a call from the recorded log.
	 * @param pId parallel Id.
	 * @param lId location id.
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN testForMatch (
    		uint64_t pId,
    		uint64_t lId
    		) = 0;
}; /*class I_MessageReproducer*/

#endif /*I_MESSAGEREPRODUCER_H*/
