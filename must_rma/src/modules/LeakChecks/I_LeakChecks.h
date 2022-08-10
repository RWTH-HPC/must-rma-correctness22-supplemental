/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_LeakChecks.h
 *       @see I_LeakChecks.
 *
 *  @date 17.05.2011
 *  @author Mathias Korepkat, Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"
#include "I_ChannelId.h"

#ifndef I_LEAKCHECKS_H
#define I_LEAKCHECKS_H

/**
 * Checks whether MPI resources were leaked when MPI_Finalize
 * is called.
 *
 * Dependencies (order as listed):
 * - ParallelIdAnalysis
 * - CreateMessage
 * - CommTrack
 * - DatatypeTrack
 * - ErrTrack
 * - GroupTrack
 * - KeyvalTrack
 * - OpTrack
 * - RequestTrack
 *
 */
class I_LeakChecks : public gti::I_Module
{
public:

	/**
	 * Notification of a set of MPI_Finalize calls.
	 *
	 * @param thisChannel channel id of completion event, or NULL if all processes connected to this place completed.
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN finalizeNotify (gti::I_ChannelId *thisChannel) = 0;
};/*class I_LeakChecks*/

#endif /*I_LEAKCHECKS_H*/
