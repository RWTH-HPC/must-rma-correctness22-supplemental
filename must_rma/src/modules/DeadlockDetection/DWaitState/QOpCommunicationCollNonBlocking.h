/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file QOpCommunicationColl.h
 *       @see must::QOpCommunicationColl.
 *
 *  @date 28.07.2015
 *  @author Tobias Hilbrich
 */

#ifndef QOPCOMMUNICATIONCOLLNONBLOCKING_H
#define QOPCOMMUNICATIONCOLLNONBLOCKING_H

#include "QOpCommunicationColl.h"

using namespace gti;

namespace must
{
    /**
     * A non blocking collective communication operation.
     */
    class QOpCommunicationCollNonBlocking : public QOpCommunicationColl
    {
    public:

        /**
         * Constructor for a non blocking collective communication operation.
         * @see QOpCommunicationColl::QOpCommunicationColl
         * @param request of the collective.
         */
        QOpCommunicationCollNonBlocking (
                DWaitState *dws,
                MustParallelId pId,
                MustLocationId lId,
                MustLTimeStamp ts,
                I_CommPersistent *comm,
                MustCollCommType collType,
                MustLTimeStamp waveNumberInComm,
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
         * @see QOp::blocks
         */
        virtual bool blocks (void);

    protected:
        MustRequestType myRequest;

        /**
         * Destructor.
         */
        virtual ~QOpCommunicationCollNonBlocking (void);
    };

} /*namespace must*/

#endif /*QOPCOMMUNICATIONCOLLNONBLOCKING_H*/
