/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file CommReduction.h
 *       @see MUST::CommReduction.
 *
 *  @date 04.03.2011
 *  @author Mathias Korepkat, Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "CompletionTree.h"

#include "I_CommReduction.h"

#ifndef COMMREDUCTION_H
#define COMMREDUCTION_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_CommReduction.
     */
    class CommReduction : public gti::ModuleBase<CommReduction, I_CommReduction>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		CommReduction (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~CommReduction (void);

    		/**
    		 * @see I_CommReduction::reduce
    		 */
    		GTI_ANALYSIS_RETURN reduce (
                    MustParallelId pId,
    				int reachableBegin,
    				int reachableEnd,
    				int worldSize,
    				MustCommType commNull,
    				MustCommType commSelf,
    				MustCommType commWorld,
    				int numWorlds,
    				MustCommType *worlds,
    				gti::I_ChannelId *thisChannel,
    				std::list<gti::I_ChannelId*> *outFinishedChannels);

    		/**
    		 * The timeout function, see gti::I_Reduction.timeout
    		 */
    		void timeout (void);

    protected:

    		std::map<int, MustCommType> myWorlds;

    		int maxRank; /**< maximum rank in reachable ranks.*/
    		int minRank; /**< minimum rank in reachable ranks.*/
    		std::list<I_ChannelId*> myReductionPartners; /**< list of reduction partners.*/
    		/**
    		 * Completion tree, IMPORTANT: we only do one single reduction here,
    		 * it either fails or succeeds, so we don't need a history of timed-out
    		 * reductions here.
    		 */
    		CompletionTree *myCompletion;
    		bool myTimedOut;
    		bool myWasSuccessful;
    };
} /*namespace MUST*/

#endif /*COMMREDUCTION_H*/
