/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_CollMatchListener.h
 *       @see I_CollMatchListener.
 *
 *  @date 10.08.2011
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#include "BaseIds.h"
#include "MustEnums.h"
#include "MustTypes.h"
#include "I_Comm.h"

#ifndef I_COLLMATCHLISTENER_H
#define I_COLLMATCHLISTENER_H

namespace must
{
    /**
     * Interface to be implemented by listeners on collective matches.
     */
    class I_CollMatchListener
    {
    public:

        /**
         * Called when a new collective is completly matched.
         * @param comm only valid until this call returns.
         */
        virtual void newMatch (
                MustCollCommType collId,
                I_Comm* comm) = 0;

    };/*class I_CollMatchListener*/
} /*namespace must*/

#endif /*I_COLLMATCHLISTENER_H*/
