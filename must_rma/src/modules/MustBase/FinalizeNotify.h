/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file FinalizeNotify.h
 *       @see MUST::FinalizeNotify.
 *
 *  @date 04.04.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"

#include "I_FinalizeNotify.h"

#ifndef FINALIZENOTIFY_H
#define FINALIZENOTIFY_H

using namespace gti;

namespace must
{
	/**
     * Template for correctness checks interface implementation.
     */
    class FinalizeNotify : public gti::ModuleBase<FinalizeNotify, I_FinalizeNotify>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		FinalizeNotify (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~FinalizeNotify (void);

    		/**
    		 * @see I_FinalizeNotify::notify.
    		 */
    		GTI_ANALYSIS_RETURN notify ();

    protected:
    };
} /*namespace MUST*/

#endif /*FINALIZENOTIFY_H*/
