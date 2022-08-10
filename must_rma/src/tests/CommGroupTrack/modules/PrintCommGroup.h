/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file PrintCommGroup.h
 *       @see MUST::PrintCommGroup.
 *
 *  @date 06.03.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_PrintCommGroup.h"
#include "I_CreateMessage.h"

#ifndef PRINTCOMMGROUP_H
#define PRINTCOMMGROUP_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_PrintCommGroup.
     */
    class PrintCommGroup : public gti::ModuleBase<PrintCommGroup, I_PrintCommGroup>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		PrintCommGroup (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~PrintCommGroup (void);

    		/**
    		 * @see I_PrintCommGroup::printComm.
    		 */
    		GTI_ANALYSIS_RETURN printComm (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustCommType comm);

    		/**
    		 * @see I_PrintCommGroup::printGroup.
    		 */
    		GTI_ANALYSIS_RETURN printGroup (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustGroupType group);

    protected:
    		I_LocationAnalysis* myLocations;
    		I_CreateMessage* myLogger;
    		I_CommTrack* myCTracker;
    		I_GroupTrack* myGTracker;

    		/**
    		 * Adds information for the mapping of the given group to the
    		 * stream.
    		 *
    		 * @param group to print.
    		 * @param stream to print to.
    		 */
    		void addGroupInfoToStream (I_GroupTable* group, std::stringstream &stream);
    }; /*class PrintCommGroup */
} /*namespace MUST*/

#endif /*PRINTCOMMGROUP_H*/
