/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file PrintErr.h
 *       @see MUST::PrintErr.
 *
 *  @date 12.05.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_CreateMessage.h"

#include "I_PrintErr.h"

#ifndef PRINTERR_H
#define PRINTERR_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_PrintErr.
     */
    class PrintErr : public gti::ModuleBase<PrintErr, I_PrintErr>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		PrintErr (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~PrintErr (void);

    		/**
    		 * @see I_PrintErr::print.
    		 */
    		GTI_ANALYSIS_RETURN print (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustErrType err);

    protected:
    		I_CreateMessage* myLogger;
    		I_ErrTrack* myErrTracker;
    		I_LocationAnalysis* myLocations;
    }; /*class PrintErr */
} /*namespace MUST*/

#endif /*PRINTERR_H*/
