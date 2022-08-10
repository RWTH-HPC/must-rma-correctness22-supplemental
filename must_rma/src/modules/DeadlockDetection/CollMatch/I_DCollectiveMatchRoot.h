/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_DCollectiveMatchRoot.h
 *       @see I_DCollectiveMatchRoot.
 *
 *  @date 25.04.2012
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat, Fabian Haensel
 */

#ifndef I_COLLECTIVEMATCHROOT_H
#define I_COLLECTIVEMATCHROOT_H

#include "I_Reduction.h"
#include "I_DCollectiveMatch.h"

using namespace must;

/**
 * Interface for distributed collective matching.
 * Version for reduction running on the TBON
 * root.
 */
class I_DCollectiveMatchRoot : public I_DCollectiveMatch, public gti::I_Reduction
{
public:
    virtual ~I_DCollectiveMatchRoot() {};

    /**
     * We listen to timeouts to trigger flush requests if nothing happens.
     */
    virtual void timeout (void) = 0;
};

#endif /*I_COLLECTIVEMATCHROOT_H*/
