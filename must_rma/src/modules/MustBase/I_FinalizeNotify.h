/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_FinalizeNotify.h
 *       @see I_FinalizeNotify.
 *
 *  @date 04.04.2011
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"

#ifndef I_FINALIZENOTIFY_H
#define I_FINALIZENOTIFY_H

/**
 * Triggers a notifcation event on the API call finalizeNotify when MPI_Finalize
 * is issued, must be placed on all application processes for MUST based tools.
 *
 * Dependencies (order as listed):
 *
 */
class I_FinalizeNotify : public gti::I_Module
{
public:

	/**
	 * Triggers the notification, must be mapped to MPI_Finalize pre.
	 *
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN notify () = 0;
};/*class I_FinalizeNotify*/

#endif /*I_FINALIZENOTIFY_H*/
