/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file PrintRequest.h
 *       @see MUST::PrintRequest.
 *
 *  @date 04.02.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_PrintRequest.h"
#include "I_CreateMessage.h"

#ifndef PRINTREQUEST_H
#define PRINTREQUEST_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_PrintRequest.
     */
    class PrintRequest : public gti::ModuleBase<PrintRequest, I_PrintRequest>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		PrintRequest (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~PrintRequest (void);

    		/**
    		 * @see I_PrintRequest::print.
    		 */
    		GTI_ANALYSIS_RETURN print (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustRequestType request);

    protected:
    		I_CreateMessage* myLogger;
    		I_RequestTrack* myRTracker;
    		I_LocationAnalysis* myLocations;
    }; /*class PrintRequest */
} /*namespace MUST*/

#endif /*PRINTREQUEST_H*/
