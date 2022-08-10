/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_FinalizeMust.h
 *       @see I_FinalizeMust.
 *
 *  @date 05.03.2013
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"

#ifndef I_FINALIZEMUST_H
#define I_FINALIZEMUST_H

/**
 * Trigers the actual finalize event once we get informed of the finalizeNotify event.
 * This is since MPI_Finalize now is only a local finalizer, i.e. it shuts down the application
 * layer, but the remainder of the TBON is now either shut down with this module (when
 * we see the last MPI_Finalize) or with DWaitState, which has its own logic.
 *
 * Dependencies (order as listed):
 *
 */
class I_FinalizeMust : public gti::I_Module
{
public:

	/**
	 * Trigers us to generate the finalize event we define in BaseApi.h.
	 *
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN notify () = 0;
};/*class I_FinalizeMust*/

#endif /*I_FINALIZEMUST_H*/
