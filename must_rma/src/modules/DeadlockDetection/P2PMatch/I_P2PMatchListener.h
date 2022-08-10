/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_P2PMatchListener.h
 *       @see I_P2PMatchListener.
 *
 *  @date 10.08.2011
 *  @author Tobias Hilbrich, Mathias Korepkat, Joachim Protze
 */

#include "BaseIds.h"
#include "MustEnums.h"
#include "MustTypes.h"

#ifndef I_P2PMATCHLISTENER_H
#define I_P2PMATCHLISTENER_H

namespace must
{
    /**
     * Interface to be implemented by listeners on P2P matches.
     */
    class I_P2PMatchListener
    {
    public:
        /**
         * Called when a new P2P match occurs.
         */
        virtual void newMatch (
                int sendRankWorld,
                int receiveRankWorld,
                bool sendHasRequest,
                MustRequestType sendRequest,
                bool receiveHasRequest,
                MustRequestType receiveRequest,
                must::MustSendMode sendMode) = 0;

    };/*class I_P2PMatchListener*/
} /*namespace must*/

#endif /*I_P2PMATCHLISTENER_H*/
