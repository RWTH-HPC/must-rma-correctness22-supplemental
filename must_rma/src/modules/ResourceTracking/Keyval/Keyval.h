/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_Keyval.h
 *       @see I_Keyval.
 *
 *  @date 15.07.2011
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#include "I_Keyval.h"
#include "HandleInfoBase.h"

#include <string>

#ifndef KEYVAL_H
#define KEYVAL_H

namespace must
{
    /**
     * Implementation of I_Comm (and I_CommPersistent).
     */
    class Keyval : public I_KeyvalPersistent, public HandleInfoBase
    {
    public:
        /**
         * Constructor.
         * Initializes as a MPI_KEYVAL_INVALID info.
         */
        Keyval ();

        /**
         * Constructor.
         * Initializes as a predefined keyvalue.
         */
        Keyval (MustMpiKeyvalPredefined predefined, std::string predefinedName);

        /**
         * Destructor.
         */
        ~Keyval ();

        bool isNull (void); /**< @see I_Keyval::.*/
        bool isPredefined (void); /**< @see I_Keyval::.*/
        MustParallelId getCreationPId (void); /**< @see I_Keyval::.*/
        MustLocationId getCreationLId (void); /**< @see I_Keyval::.*/
        MustMpiKeyvalPredefined getPredefinedInfo (void); /**< @see I_Keyval::.*/
        std::string getPredefinedName (void); /**< @see I_Keyval::.*/
        bool printInfo (
                std::stringstream &out,
                std::list<std::pair<MustParallelId,MustLocationId> > *pReferences); /**< @see I_Keyval::.*/

        std::string getResourceName (void); /**< @see HandleInfoBase::getResourceName.*/

    public:
        MustMpiKeyvalPredefined myPredefined;
        std::string myPredefinedName;

        bool myIsNull;
        bool myIsPredefined;
        MustParallelId myCreationPId;
        MustLocationId myCreationLId;

    };/*class Keyval*/
}/*namespace must*/

#endif /*KEYVAL_H*/

