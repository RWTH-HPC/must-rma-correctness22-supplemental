/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_PrintOp.h
 *       @see I_PrintOp.
 *
 *  @date 10.05.2011
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h" //TODO Needs to be renamed to GTI enums
#include "BaseIds.h"
#include "I_OpTrack.h"
#include "I_LocationAnalysis.h"

#ifndef I_PRINTOP_H
#define I_PRINTOP_H

/**
 * Interface for a module that prints all available
 * information on a request, using I_OpTrack.
 *
 * Dependencies:
 *  1) LocationAnalysis
 *  2) CreateMessage
 *  3) OpTrack
 */
class I_PrintOp : public gti::I_Module
{
public:
	/**
	 * Prints information on the given op as log events.
	 * @param pId parallel Id of the print call.
	 * @param lId location Id of the print call.
	 * @param op to print.
	 * @return @see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN print (
    		MustParallelId pId,
    		MustLocationId lId,
    		MustOpType op) = 0;

}; /*class I_PrintOp*/

#endif /*I_PRINTOP_H*/
