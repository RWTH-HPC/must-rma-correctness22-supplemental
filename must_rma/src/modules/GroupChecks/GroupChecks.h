/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file GroupChecks.h
 *       @see MUST::GroupChecks.
 *
 *  @date 23.05.2011
 *  @author Mathias Korepkat, Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"
#include "I_ArgumentAnalysis.h"
#include "I_CreateMessage.h"
#include "I_CommTrack.h"
#include "I_GroupTrack.h"

#include "I_GroupChecks.h"

#include <string>

#ifndef GROUPCHECKS_H
#define GROUPCHECKS_H

using namespace gti;

namespace must
{
	/**
     * GroupChecks for correctness checks interface implementation.
     */
    class GroupChecks : public gti::ModuleBase<GroupChecks, I_GroupChecks>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		GroupChecks (const char* instanceName);

    	/**
    	 * Destructor.
    	 */
    		virtual ~GroupChecks (void);

    	/**
    	 * @see I_GroupChecks::errorIfNotKnown.
    	 */
    		GTI_ANALYSIS_RETURN errorIfNotKnown (
    			MustParallelId pId,
    			MustLocationId lId,
    			int aId,
    			MustGroupType group
    			);

    	/**
    	 * @see I_GroupChecks::errorIfNull.
    	 */
    		GTI_ANALYSIS_RETURN errorIfNull (
    			MustParallelId pId,
    			MustLocationId lId,
    			int aId,
    			MustGroupType group
    			);

        /**
         * @see I_GroupChecks::errorIfIntegerGreaterGroupSize.
         */
        	GTI_ANALYSIS_RETURN errorIfIntegerGreaterGroupSize (
        		MustParallelId pId,
        		MustLocationId lId,
        		int aId_val,
        		int aId_grp,
        		int value,
        		MustGroupType group
        		);

        /**
         * @see I_GroupChecks::errorIfIntegerArrayElementGreaterGroupSize.
         */
           	GTI_ANALYSIS_RETURN errorIfIntegerArrayElementGreaterGroupSize (
           		MustParallelId pId,
           		MustLocationId lId,
           		int aId_val,
           		int aId_grp,
           		const int* array,
           		int size,
           		MustGroupType group
           		);

       /**
        * @see I_GroupChecks::errorIfRankFromRangesNotInGroup.
        */
        	GTI_ANALYSIS_RETURN errorIfRankFromRangesNotInGroup (
           		MustParallelId pId,
          		MustLocationId lId,
           		int aId_val,
           		int aId_grp,
           		const int* array,
           		int size,
           		MustGroupType group
           		);

       /**
        * @see I_GroupChecks::warningIfEmpty.
        */
           	GTI_ANALYSIS_RETURN warningIfEmpty (
           		MustParallelId pId,
           		MustLocationId lId,
           		int aId,
           		MustGroupType group
           		);

       /**
        * @see I_GroupChecks::warningIfNull.
        */
           	GTI_ANALYSIS_RETURN warningIfNull (
           		MustParallelId pId,
           		MustLocationId lId,
           		int aId,
           		MustGroupType group
           		);

     /**
      * @see I_GroupChecks::errorRankNotInComm.
      */
           	GTI_ANALYSIS_RETURN errorRankNotInComm (
           		MustParallelId pId,
           		MustLocationId lId,
           		int aId_grp,
           		int aId_comm,
           		MustGroupType group,
        		MustCommType comm
           		);
    protected:
    		I_ParallelIdAnalysis* myPIdMod;
    	    I_CreateMessage* myLogger;
    	    I_ArgumentAnalysis* myArgMod;
    	    I_CommTrack* myCommMod;
    	    I_GroupTrack* myGroupMod;
    };
} /*namespace MUST*/

#endif /*GROUPCHECKS_H*/
