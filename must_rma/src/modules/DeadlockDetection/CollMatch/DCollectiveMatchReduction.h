/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DCollectiveMatchReduction.h
 *       @see must::DCollectiveMatchReduction.
 *
 *  @date 25.04.2012
 *  @author Fabian Haensel, Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#ifndef DCOLLECTIVEMATCHREDUCTION_H
#define DCOLLECTIVEMATCHREDUCTION_H

#include "ModuleBase.h"

#include "I_DCollectiveMatchReduction.h"
#include "DCollectiveMatch.h"

using namespace gti;

namespace must
{
	/**
     * Instantiation of distributed collective matching and verification.
     * This is the reduction part that matches and verifies across
     * all TBON layers.
     */
    class DCollectiveMatchReduction : public DCollectiveMatch<DCollectiveMatchReduction, I_DCollectiveMatchReduction>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
        DCollectiveMatchReduction (const char* instanceName);

		/**
		 * Destructor.
		 */
		virtual ~DCollectiveMatchReduction (void);

    };
} /*namespace MUST*/

#endif /*DCOLLECTIVEMATCHREDUCTION_H*/
