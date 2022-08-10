/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file ParallelIdImpl.h
 *       Implementation for the parallel id analysis interface.
 *
 *  @date 07.01.2010
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"

#ifndef PARALLELIMPL_H
#define PARALLELIMPL_H

using namespace gti;

namespace must
{
	/**
     * Implementation for the parallel id analysis.
     */
    class ParallelIdImpl : public gti::ModuleBase<ParallelIdImpl, I_ParallelIdAnalysis, false>
    {
    protected:

    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		ParallelIdImpl (const char* instanceName);

        /**
         * @see I_ParallelIdAnalysis::getInfoForId.
         */
    		ParallelInfo getInfoForId (MustParallelId id);

    		/**
    		 * @see I_ParallelIdAnalysis::toString.
    		 */
    		std::string toString (MustParallelId id);
    }; /*class ParallelIdImpl */
} /*namespace MUST*/

#endif /*PARALLELIMPL_H*/
