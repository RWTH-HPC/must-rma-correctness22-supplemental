/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file FinalizeMust.h
 *       @see MUST::FinalizeMust.
 *
 *  @date 05.03.2013
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "BaseApi.h"

#include "I_FinalizeMust.h"

#ifndef FINALIZEMUST_H
#define FINALIZEMUST_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_FinalizeMust.
     */
    class FinalizeMust : public gti::ModuleBase<FinalizeMust, I_FinalizeMust>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		FinalizeMust (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~FinalizeMust (void);

    		/**
    		 * @see I_FinalizeMust::notify.
    		 */
    		GTI_ANALYSIS_RETURN notify ();

    protected:
    };
} /*namespace MUST*/

#endif /*FINALIZEMUST_H*/
