/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file CommPredefs.h
 *       @see MUST::CommPredefs.
 *
 *  @date 04.03.2011
 *  @author Mathias Korepkat, Tobias Hilbrich
 */

#include "ModuleBase.h"

#include "I_CommPredefs.h"

#ifndef COMMPREDEFS_H
#define COMMPREDEFS_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_CommPredefs.
     */
    class CommPredefs : public gti::ModuleBase<CommPredefs, I_CommPredefs>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		CommPredefs (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~CommPredefs (void);

    		/**
    		 * @see I_CommPredefs::propagate.
    		 */
    		GTI_ANALYSIS_RETURN propagate (
                MustParallelId pId
            );

    protected:

    };
} /*namespace MUST*/

#endif /*COMMPREDEFS_H*/
