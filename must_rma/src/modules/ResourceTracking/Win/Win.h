/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_Win.h
 *       @see I_Win.
 *
 *  @date 26.04.2017
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat, Simon Schwitanski
 */

#include "I_Win.h"
#include "HandleInfoBase.h"

#ifndef WIN_H
#define WIN_H

namespace must
{
    /**
     * Implementation of I_Win (and I_WinPersistent).
     */
    class Win : public I_WinPersistent, public HandleInfoBase
    {
    public:
        /**
         * Constructor.
         * Initializes as a MPI_WIN_NULL info.
         */
        Win ();

        /**
         * Destructor.
         */
        ~Win ();

        MUST_WIN_KIND getKind (void); /**< @see I_Win::getKind.*/
        MUST_WIN_MEMORY_MODEL getMemoryModel (void); /**< @see I_Win::getMemoryModel.*/

        I_CommPersistent* getComm (void); /**< @see I_Win::getComm.*/
        MustCommType getCommHandle (void); /**< @see I_Win::getCommHandle.*/
        MustParallelId getCreationPId (void);/**< @see I_Win::getCreationPId.*/
        MustLocationId getCreationLId (void);/**< @see I_Win::getCreationLId.*/

        MustAddressType getBase (void); /**< @see I_Win::getBase.*/ 
        int getDispUnit (void); /**< @see I_getDispUnit.*/ 

        unsigned long long getContextId (void); /**< @see I_Win::contextId.*/

        virtual MustMemIntervalListType& getMemIntervals (void);/**< @see I_Win::getMemIntervals.*/

        bool compareWins (I_Win* other); /**< @see I_Win::compareWins.*/
        bool operator == (I_Win &other); /**< @see I_Win::operator==.*/
        bool operator != (I_Win &other); /**< @see I_Win::operator!=.*/

        bool printInfo (
                std::stringstream &out,
                std::list<std::pair<MustParallelId,MustLocationId> > *pReferences); /**< @see I_Win::printInfo.*/

        std::string getResourceName (void); /**< @see HandleInfoBase::getResourceName.*/

    public:
        MUST_WIN_KIND myKind;
        MUST_WIN_MEMORY_MODEL myMemoryModel;

        I_CommPersistent* myComm;
        MustCommType myCommHandle;
        MustParallelId myCreationPId;
        MustLocationId myCreationLId;

        // Identification id used for matching windows across processes.
        // Two windows w1 and w2 are equal iff they belong to the same communicator
        // and their context ids are equal.
        unsigned long long myContextId;

        MustAddressType myBase;
        int myDispUnit;
        MustMemIntervalListType myMemIntervals;
    };/*class Win*/
}/*namespace must*/

#endif /*WIN_H*/

