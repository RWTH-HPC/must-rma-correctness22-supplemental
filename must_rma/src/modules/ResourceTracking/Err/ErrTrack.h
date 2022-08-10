/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file ErrTrack.h
 *       @see MUST::ErrTrack
 *
 *  @date 12.05.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "TrackBase.h"

#include "I_ErrTrack.h"
#include "Err.h"

#include <map>

#ifndef ERRTRACK_H
#define ERRTRACK_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_ErrTrack.
     */
    class ErrTrack : public TrackBase<Err, I_Err, MustErrType, MustMpiErrPredefined, ErrTrack, I_ErrTrack>
    {
    protected:

    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		ErrTrack (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~ErrTrack (void);

    		/**
    		 * @see I_ErrTrack::errCreate
    		 */
    		GTI_ANALYSIS_RETURN errCreate (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustErrType newErr);

    		/**
    		 * @see I_ErrTrack::errFree
    		 */
    		GTI_ANALYSIS_RETURN errFree (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustErrType err);

    		/**
    		 * @see I_ErrTrack::getErr
    		 */
    		I_Err* getErr (
    				MustParallelId pId,
    				MustErrType err);

    		/**
    		 * @see I_ErrTrack::getErr
    		 */
    		I_Err* getErr (
    				int rank,
    				MustErrType err);

    		/**
    		 * @see I_ErrTrack::getPersistentErr
    		 */
    		I_ErrPersistent* getPersistentErr (
    		        MustParallelId pId,
    		        MustErrType err);

    		/**
    		 * @see I_ErrTrack::getPersistentErr
    		 */
    		I_ErrPersistent* getPersistentErr (
    		        int rank,
    		        MustErrType err);

    protected:

            /**
             * Returns the name for a predefined errorhandle.
             */
            std::string getPredefinedName (MustMpiErrPredefined predefined);

            /**
             * Used to initialize null and predefined infos.
             * @see TrackBase::createPredefinedInfo.
             * (Implementation of hook)
             */
            Err* createPredefinedInfo (int predef, MustErrType handle);

    }; /*class ErrTrack */
} /*namespace MUST*/

#endif /*ERRTRACK_H*/
