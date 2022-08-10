/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DatatypePredefs.h
 *       @see MUST::DatatypePredefs.
 *
 *  @date 18.02.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_DatatypePredefs.h"

#ifndef DATATYPEPREDEFS_H
#define DATATYPEPREDEFS_H

using namespace gti;

namespace must
{
	/**
     * Implementation for I_DatatypePredefs.
     */
    class DatatypePredefs : public gti::ModuleBase<DatatypePredefs, I_DatatypePredefs>
    {
    protected:

    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		DatatypePredefs (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~DatatypePredefs (void);

    		/**
    		 * @see I_DatatypePredefs::propagate.
    		 */
    		GTI_ANALYSIS_RETURN propagate (
                MustParallelId pId
            );

    }; /*class DatatypePredefs */
} /*namespace MUST*/

#endif /*DATATYPEPREDEFS_H*/
