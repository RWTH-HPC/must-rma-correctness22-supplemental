/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_Group.h
 *       @see I_Group.
 *
 *  @date 15.07.2011
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#include "I_Group.h"
#include "HandleInfoBase.h"

#ifndef GROUP_H
#define GROUP_H

namespace must
{
    /**
     * Implementation of I_Comm (and I_CommPersistent).
     */
    class Group : public I_GroupPersistent, public HandleInfoBase
    {
    public:
        /**
         * Constructor.
         * Initializes as a MPI_GROUP_NULL info.
         */
        Group ();

        /**
         * Destructor.
         */
        ~Group ();

        bool isNull (void); /**< @see I_Comm::isNull.*/
        bool isEmpty (void); /**< @see I_Comm::isEmpty.*/
        I_GroupTable* getGroup (void); /**< @see I_Comm::getGroup.*/

        MustParallelId getCreationPId (void); /**< @see I_Comm::gatCreationPId.*/
        MustLocationId getCreationLId (void); /**< @see I_Comm::getCreationLId.*/

        bool printInfo (
                std::stringstream &out,
                std::list<std::pair<MustParallelId,MustLocationId> > *pReferences); /**< @see I_Comm::printInfo.*/

        std::string getResourceName (void); /**< @see HandleInfoBase::getResourceName.*/

    public:
        bool myIsNull;
        bool myIsEmpty;

        MustParallelId myCreationPId;
        MustLocationId myCreationLId;

        I_GroupTable* myGroup;

    };/*class Group*/
}/*namespace must*/

#endif /*GROUP_H*/

