/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file WcUpdate.h
 *       @see MUST::WcUpdate.
 *
 *  @date 15.03.2011
 *  @author Tobias Hilbrich, Mathias Korepkat
 */

#include "ModuleBase.h"
#include "I_RequestTrack.h"

#include "I_WcUpdate.h"

#include <string>

#ifndef WCUPDATE_H
#define WCUPDATE_H

using namespace gti;

namespace must
{
	/**
     * Template for correctness checks interface implementation.
     */
    class WcUpdate : public gti::ModuleBase<WcUpdate, I_WcUpdate>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		WcUpdate (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~WcUpdate (void);

    		/**
    		 * @see I_WcUpdate::addPredefineds
    		 */
    		GTI_ANALYSIS_RETURN addPredefineds (
    				int anySource);

    		/**
    		 * @see I_WcUpdate::recvPost
    		 */
    		GTI_ANALYSIS_RETURN recvPost (
    				MustParallelId pId,
    				MustLocationId lId,
    				int source,
    				int statusSource);

    		/**
    		 * @see I_WcUpdate::irecv
    		 */
    		GTI_ANALYSIS_RETURN irecv (
    				MustParallelId pId,
    				MustLocationId lId,
    				int source,
    				MustRequestType request);

    		/**
    		 * @see I_WcUpdate::startPersistent
    		 */
    		GTI_ANALYSIS_RETURN startPersistent (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustRequestType request);

    		/**
    		 * @see I_WcUpdate::startPersistentArray
    		 */
    		GTI_ANALYSIS_RETURN startPersistentArray (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustRequestType* requests,
    				int count);

    		/**
    		 * @see I_WcUpdate::complete
    		 */
    		GTI_ANALYSIS_RETURN complete (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustRequestType request,
    				int flag,
    				int statusSource);

    		/**
    		 * @see I_WcUpdate::completeAny
    		 */
    		GTI_ANALYSIS_RETURN completeAny (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustRequestType* requests,
    				int count,
    				int index,
    				int flag,
    				int statusSource);

    		/**
    		 * @see I_WcUpdate::completeArray
    		 */
    		GTI_ANALYSIS_RETURN completeArray (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustRequestType *requests,
    				int count,
    				int flag,
    				int* statusSources);

    		/**
    		 * @see I_WcUpdate::completeSome
    		 */
    		GTI_ANALYSIS_RETURN completeSome (
    				MustParallelId pId,
    				MustLocationId lId,
    				MustRequestType *requests,
    				int count,
    				int *indices,
    				int numIndices,
    				int* statusSources);

    protected:
    		I_RequestTrack* myRTrack;
    		typedef std::map<MustRequestType, int> RequestMap;
    		typedef std::map<MustParallelId,  RequestMap> AllMaps;
    		AllMaps myWcReqs;
    		int myAnySource;
    };
} /*namespace MUST*/

#endif /*TEMPLATE_H*/
