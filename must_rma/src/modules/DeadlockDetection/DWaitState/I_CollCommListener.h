/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_CollCommListener.h
 *       @see I_CollCommListener
 *
 *  @date 05.03.2013
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"

#ifndef I_COLLCOMMLISTENER_H
#define I_COLLCOMMLISTENER_H

namespace must
{
    /**
     * Interface for listener communicators being used in collectives.
     */
    class I_CollCommListener
    {
    public:

        /**
         * Notification of a new communicator in use.
         * Should only be called once for each communicator
         * and all its variants on other ranks.
         *
         * Interface provides I_CommPersistent, if the implementer
         * of this callback wants to keep the communicator it must
         * increment its reference count (and decrement it when
         * it does not needs it anymore)
         *
         * @param pId of the communicator.
         * @param comm that is new.
         */
        virtual void newCommInColl (
                MustParallelId pId,
                I_CommPersistent* comm) = 0;

    };/*class I_CollCommListener*/
}

#endif /*I_COLLCOMMLISTENER_H*/
