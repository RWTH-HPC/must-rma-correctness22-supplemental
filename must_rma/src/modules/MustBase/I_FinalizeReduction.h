/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_FinalizeReduction.h
 *       @see I_FinalizeReduction.
 *
 *  @date 04.04.2011
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"
#include "I_Reduction.h"

#include <list>

#ifndef I_FINALIZEREDUCTION_H
#define I_FINALIZEREDUCTION_H

/**
 * Interface for reducing multiple finalize notify events into a single one.
 * @see I_FinalizeNotify
 *
 * Dependencies (order as listed):
 * X
 */
class I_FinalizeReduction : public gti::I_Module, public gti::I_Reduction
{
public:

	/**
	 * Reduced finalizeNotify events.
    	 * @param thisChannel @see gti::I_Reduction.
    	 * @param outFinishedChannels @see gti::I_Reduction.
	 */
    virtual gti::GTI_ANALYSIS_RETURN reduce (
    		gti::I_ChannelId *thisChannel,
    		std::list<gti::I_ChannelId*> *outFinishedChannels) = 0;

};/*class I_FinalizeReduction*/

#endif /*I_FINALIZEREDUCTION_H*/
