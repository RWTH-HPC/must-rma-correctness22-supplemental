/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_Group.h
 *       @see I_Group.
 *
 *  @date 23.06.2011
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#include "MustEnums.h"
#include "BaseIds.h"
#include <iostream>
#include <list>

#include "I_GroupTable.h"
#include "I_Destructable.h"

#ifndef I_GROUP_H
#define I_GROUP_H

namespace must
{
    /**
     * Interface for storage and accessing Information
     * on a group as defined in MPI.
     */
    class I_Group
    {
    public:
        /*
         * Basic information for a group handle.
         */
        virtual bool isNull (void) = 0; /**< True if this is MPI_COMM_NULL, isKnown=true in that case, the other pieces of information are not set. */
        virtual bool isEmpty (void) = 0; /**< True if this type is MPI_GROUP_EMPTY, this is usually an isPredefined, but here we only have one predefined.*/
        //Only for user groups (isKnown && !isNull && !isEmpty)
        virtual must::I_GroupTable* getGroup (void) = 0; /**< Pointer to the process group associated with this group handle (lists the ranks and their order).*/

        virtual MustParallelId getCreationPId (void) = 0; /**< For persistent groups information for call that created the group, otherwise not set.*/
        virtual MustLocationId getCreationLId (void) = 0; /**< For persistent groups information for call that created the group, otherwise not set.*/

        /**
         * Prints information for a specified group.
         * Designed for printing in a style that suits the usage
         * of CreateMessage.
         *
         * @param out stream to use for output.
         * @param pReferences current references to which any additional references for the new handle will be added.
         * @return true if successful.
         */
        virtual bool printInfo (
                std::stringstream &out,
                std::list<std::pair<MustParallelId,MustLocationId> > *pReferences) = 0;

    };/*class I_Group*/

    /**
     * Interface for storage and accessing Information
     * on a group as defined in MPI. This is the persistent
     * version of the interface. The user needs to call I_GroupPersistent::erase
     * when he is finished with it.
     */
    class I_GroupPersistent : public I_Group, public virtual I_Destructable
    {
    };/*class I_GroupPersistent*/

}/*namespace must*/

#endif /*I_GROUP_H*/

