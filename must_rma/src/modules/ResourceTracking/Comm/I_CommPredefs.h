/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_CommPredefs.h
 *       @see I_CommPredefs.
 *
 *  @date 04.03.2011
 *  @author Mathias Korepkat, Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"

#ifndef I_COMMPREDEFS_H
#define I_COMMPREDEFS_H

/**
 * Helper module to extract predefineds for MPI communicators
 * and pass them to a wrapp-everywhere function on which
 * a reduction runs to reduce this information.
 * This information includes an interval of processes reachable
 * by this place (reachable in the GTI communication system).
 *
 * See I_DatatypePredefs for extra information on the ups and
 * downs of this design.
 *
 * Dependencies (order as listed):
 * X
 *
 */
class I_CommPredefs : public gti::I_Module
{
public:

	/**
	 * Triggers the extraction of information and the call
	 * of the wrapp-everywhere call used to forward this
	 * information.
	 *
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN propagate (
                MustParallelId pId
) = 0;
};/*class I_CommPredefs*/

#endif /*I_COMMPREDEFS_H*/
