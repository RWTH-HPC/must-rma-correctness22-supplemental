/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file Err.h
 *       @see Err.
 *
 *  @date 15.07.2011
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#include "I_Err.h"
#include "HandleInfoBase.h"

#include <string>

#ifndef ERR_H
#define ERR_H

namespace must
{
    /**
     * Implementation of I_Comm (and I_CommPersistent).
     */
    class Err : public I_ErrPersistent, public HandleInfoBase
    {
    public:
        /**
         * Constructor.
         * Initializes as a MPI_ERRORHANDLER_NULL info.
         */
        Err ();

        /**
         * Constructor.
         * Initializes as a predefined info.
         */
        Err (MustMpiErrPredefined predefined, std::string predefinedName);

        /**
         * Destructor.
         */
        ~Err ();

        bool isNull (void); /**< @see I_Err::isNull.*/
        bool isPredefined (void); /**< @see I_Err::isPredefined.*/
        MustParallelId getCreationPId (void); /**< @see I_Err::getCreationPId.*/
        MustLocationId getCreationLId (void); /**< @see I_Err::getCreationLId.*/
        MustMpiErrPredefined getPredefinedInfo (void); /**< @see I_Err::getPredefinedInfo.*/
        std::string getPredefinedName (void); /**< @see I_Err::getPredefinedName.*/
        bool printInfo (
                std::stringstream &out,
                std::list<std::pair<MustParallelId,MustLocationId> > *pReferences); /**< @see I_Err::printInfo.*/

        std::string getResourceName (void); /**< @see HandleInfoBase::getResourceName.*/

    public:
        MustMpiErrPredefined myPredefined;
        std::string myPredefinedName;

        bool myIsNull;
        bool myIsPredefined;
        MustParallelId myCreationPId;
        MustLocationId myCreationLId;

    };/*class Err*/
}/*namespace must*/

#endif /*ERR_H*/

