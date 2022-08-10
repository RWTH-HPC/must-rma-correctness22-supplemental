/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file Template.h
 *       @see MUST::Template.
 *
 *  @date 01.03.2011
 *  @author Mathias Korepkat, Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"
#include "I_ArgumentAnalysis.h"
#include "I_CreateMessage.h"

#include "I_Template.h"

#include <string>

#ifndef TEMPLATE_H
#define TEMPLATE_H

using namespace gti;

namespace must
{
	/**
     * Template for correctness checks interface implementation.
     */
    class Template : public gti::ModuleBase<Template, I_Template>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		Template (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~Template (void);

    		/**
    		 * @see I_Template::analysisFunction.
    		 */
    		GTI_ANALYSIS_RETURN analysisFunction (MustParallelId pId, MustLocationId lId, int aId);

    protected:
    		I_ParallelIdAnalysis* myPIdMod;
    	    I_CreateMessage* myLogger;
    	    I_ArgumentAnalysis* myArgMod;
    };
} /*namespace MUST*/

#endif /*TEMPLATE_H*/
