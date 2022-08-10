/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file PrintOp.h
 *       @see MUST::PrintOp.
 *
 *  @date 10.05.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_CreateMessage.h"

#include "I_PrintOp.h"

#ifndef PRINTOP_H
#define PRINTOP_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_PrintOp.
     */
    class PrintOp : public gti::ModuleBase<PrintOp, I_PrintOp>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		PrintOp (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~PrintOp (void);

    		/**
    		 * @see I_PrintOp::print.
    		 */
    		GTI_ANALYSIS_RETURN print (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustOpType op);

    protected:
    		I_CreateMessage* myLogger;
    		I_OpTrack* myOpTracker;
    		I_LocationAnalysis* myLocations;
    }; /*class PrintOp */
} /*namespace MUST*/

#endif /*PRINTOP_H*/
