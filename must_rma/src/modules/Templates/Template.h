/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file Template.h
 *       @see MUST::Template.
 *
 *  @date 21.01.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_Template.h"

#ifndef TEMPLATE_H
#define TEMPLATE_H

using namespace gti;

namespace must
{
	/**
     * Template for analysis interface implementation.
     */
    class Template : public gti::ModuleBase<Template, I_Template>
    {
    protected:

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
    		 *
    		GTI_ANALYSIS_RETURN analysisFunction (void);
    		 */

    }; /*class Template */
} /*namespace MUST*/

#endif /*TEMPLATE_H*/
