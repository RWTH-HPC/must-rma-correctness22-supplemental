/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DCollectiveMatchRoot.h
 *       @see must::DCollectiveMatchRoot.
 *
 *  @date 25.04.2012
 *  @author Fabian Haensel, Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#ifndef DCOLLECTIVEMATCHROOT_H
#define DCOLLECTIVEMATCHROOT_H

#include "ModuleBase.h"

#include "I_DCollectiveMatchRoot.h"
#include "DCollectiveMatch.h"

using namespace gti;

namespace must
{
    /**
     * Instantiation of distributed collective matching and verification.
     * This is the reduction part that does the final matching on the
     * TBON root.
     */
    class DCollectiveMatchRoot : public DCollectiveMatch<DCollectiveMatchRoot, I_DCollectiveMatchRoot>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
        DCollectiveMatchRoot (const char* instanceName);

        /**
         * Destructor.
         */
        virtual ~DCollectiveMatchRoot (void);

    };
} /*namespace MUST*/

#endif /*DCOLLECTIVEMATCHROOT_H*/
