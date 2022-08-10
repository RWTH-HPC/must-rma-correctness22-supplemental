/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_Win.h
 *       @see I_Win.
 *
 *  @date 26.04.2017
 *  @author Tobias Hilbrich, Simon Schwitanski
 */

#include "MustTypes.h"
#include "BaseIds.h"

#include "I_Comm.h"
#include "I_Destructable.h"
#include "StridedBlock.h"
#include <mpi.h>

#ifndef I_WIN_H
#define I_WIN_H

namespace must
{
    /**
     * Enumeration of the kinds of windows we handle.
     */
    enum MUST_WIN_KIND
    {
        MUST_WIN_CREATE = 0,   /**< Window was created with MPI_Win_create.*/
        MUST_WIN_DYNAMIC,   /**< Window was created with MPI_Win_dynamic.*/
        MUST_WIN_ALLOCATE,   /**< Window was created with MPI_Win_allocate.*/
        MUST_WIN_SHARED,   /**< Window was created with MPI_Win_allocate_shared.*/
        MUST_WIN_UNKNOWN /**< Undefined kind of window.*/
    };

    /**
     * Enumeration of memory model of a window.
     */
    enum MUST_WIN_MEMORY_MODEL
    {
        MUST_WIN_MEMORY_UNIFIED = MPI_WIN_UNIFIED, /**< Unified memory window. */
        MUST_WIN_MEMORY_SEPARATE = MPI_WIN_SEPARATE, /**< Separate memory window. */
        MUST_WIN_MEMORY_UNKNOWN /**< Undefined memory model. */
    };


    /**
     * Interface for storage and accessing information
     * on an RMA win as defined in MPI-3.
     */
    class I_Win
    {
    public:
        /*
         * Basic information
         */
        virtual MUST_WIN_KIND getKind (void) = 0; /**< Returns the kind of the request.*/
        virtual MUST_WIN_MEMORY_MODEL getMemoryModel (void) = 0; /**< Returns the memory model of the window.*/
        virtual MustAddressType getBase (void) = 0; /**< Returns the base pointer used at window creation.*/
        virtual int getDispUnit (void) = 0; /**< Returns the displacement unit set at window creation.*/
        virtual unsigned long long getContextId (void) = 0; /**< Returns the id that determines whether two window handles with equal communicators refer to the same window or not.*/

        /*
         * Returns the memory intervals used by the window. In case of dynamic windows, potentially
         * multiple memory intervals can be attached to the same window.
         */
        virtual MustMemIntervalListType& getMemIntervals (void) = 0;

        /*
         * Communicator associated with the window.
         */
        virtual I_CommPersistent* getComm (void) = 0;

        /*
         * Communicator handle associated with the window.
         */
        virtual MustCommType getCommHandle (void) = 0;

        /*
         * Creation information of window.
         */
        virtual MustParallelId getCreationPId (void) = 0;
        virtual MustLocationId getCreationLId (void) = 0;

        /**
         * Returns true if this window is equal to the given one.
         * (Refer to the same communicator with equal context id.)
         * Returns false otherwise.
         *
         * @param other window to compare to.
         */
        virtual bool compareWins(I_Win* other) = 0;

        /**
         * Returns true if this window is equal to the given one.
         * (Refer to the same communicator with equal context id.)
         * Returns false otherwise. This is the 'operator version' of
         * compareWins().
         *
         * @param other window to compare to.
         */
        virtual bool operator == (I_Win &other) = 0;

        /**
         * Returns false if this window is equal to the given one.
         * (Refer to the same communicator with equal context id.)
         * Returns true otherwise. This is the 'operator version' of
         * the opposite of compareWins().
         *
         * @param other window to compare to.
         */
        virtual bool operator != (I_Win &other) = 0;

        /**
         * Prints information for a specified window.
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

        /**
         * Virtual destructor as needed
         */
        virtual ~I_Win (void) {}
    };/*class I_Win*/

    /**
     * Interface for storage and accessing information
     * on a window as defined in MPI. This is the persistent
     * version of the interface. The user needs to call I_WinPersistent::erase
     * when he is finished with it.
     */
    class I_WinPersistent : public I_Win, public virtual I_Destructable
    {
    };/*class I_WinPersistent*/

}/*namespace must*/

#endif /*I_WIN_H*/

