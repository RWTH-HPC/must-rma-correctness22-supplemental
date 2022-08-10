/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file QOpCommunicationP2PNonBlocking.h
 *       @see must::QOpCommunicationP2PNonBlocking.
 *
 *  @date 01.03.2013
 *  @author Tobias Hilbrich
 */

#ifndef QOPCOMMUNICATIONP2PNONBLOCKING_H
#define QOPCOMMUNICATIONP2PNONBLOCKING_H

#include "MustEnums.h"
#include "BaseIds.h"
#include "MustTypes.h"

#include "QOpCommunicationP2P.h"

using namespace gti;

namespace must
{
    /**
     * A non-blocking P2P communication operation.
     */
    class QOpCommunicationP2PNonBlocking : public QOpCommunicationP2P
    {
    public:

        /**
         * Constructor for any P2P communication operation.
         * @see QOpCommunicationP2P::QOpCommunicationP2P
         * @param request associated with this non-blocking P2P communication.
         */
        QOpCommunicationP2PNonBlocking (
                DWaitState *dws,
                MustParallelId pId,
                MustLocationId lId,
                MustLTimeStamp ts,
                I_CommPersistent *comm,
                bool isSend,
                int sourceTarget,
                bool isWc,
                MustSendMode mode,
                int tag,
                MustRequestType request
        );

        /**
         * @see QOpCommunication::hasRequest
         * Returns true.
         */
        virtual bool hasRequest (void);

        /**
         * @see QOpCommunication::getRequest
         * Returns the request.
         */
        virtual MustRequestType getRequest (void);

        /**
         * @see QOp::printVariablesAsLabelString
         */
        virtual std::string printVariablesAsLabelString (void);

        /**
         * @see QOpCommunicationP2P
         */
        virtual bool isNonBlockingP2P (void);

    protected:
        MustRequestType myRequest;

        /**
         * Destructor.
         */
        virtual ~QOpCommunicationP2PNonBlocking (void);
    };

} /*namespace must*/

#endif /*QOPCOMMUNICATIONP2PNONBLOCKING_H*/
