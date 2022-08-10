/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file PrintKeyval.h
 *       @see MUST::PrintKeyval.
 *
 *  @date 12.05.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_CreateMessage.h"

#include "I_PrintKeyval.h"

#ifndef PRINTKEYVAL_H
#define PRINTKEYVAL_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_PrintKeyval.
     */
    class PrintKeyval : public gti::ModuleBase<PrintKeyval, I_PrintKeyval>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		PrintKeyval (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~PrintKeyval (void);

    		/**
    		 * @see I_PrintKeyval::print.
    		 */
    		GTI_ANALYSIS_RETURN print (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustKeyvalType keyval);

    protected:
    		I_CreateMessage* myLogger;
    		I_KeyvalTrack* myKeyvalTracker;
    		I_LocationAnalysis* myLocations;
    }; /*class PrintKeyval */
} /*namespace MUST*/

#endif /*PRINTKEYVAL_H*/
