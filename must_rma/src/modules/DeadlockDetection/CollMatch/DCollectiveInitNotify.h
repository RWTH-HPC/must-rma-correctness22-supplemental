/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DCollectiveInitNotify.h
 *       @see MUST::DCollectiveInitNotify.
 *
 *  @date 03.05.2012
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat, Fabian Haensel
 */

#include "ModuleBase.h"
#include "I_DCollectiveInitNotify.h"

#ifndef DCOLLECTIVEINITNOTIFY_H
#define DCOLLECTIVEINITNOTIFY_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_DCollectiveInitNotify.
     */
    class DCollectiveInitNotify : public gti::ModuleBase<DCollectiveInitNotify, I_DCollectiveInitNotify>
    {
    protected:

    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
        DCollectiveInitNotify (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~DCollectiveInitNotify (void);

    		/**
    		 * @see I_DCollectiveInitNotify::notifyInit
    		 */
    		GTI_ANALYSIS_RETURN notifyInit (void);

    }; /*class DCollectiveInitNotify */
} /*namespace MUST*/

#endif /*DCOLLECTIVEINITNOTIFY_H*/
