/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_PrintKeyval.h
 *       @see I_PrintKeyval.
 *
 *  @date 12.05.2011
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h" //TODO Needs to be renamed to GTI enums
#include "BaseIds.h"
#include "I_KeyvalTrack.h"
#include "I_LocationAnalysis.h"

#ifndef I_PRINTKEYVAL_H
#define I_PRINTKEYVAL_H

/**
 * Interface for a module that prints all available
 * information on keyvalue, using I_KeyvalTrack.
 *
 * Dependencies:
 *  1) LocationAnalysis
 *  2) CreateMessage
 *  3) KeyvalTrack
 */
class I_PrintKeyval : public gti::I_Module
{
public:
	/**
	 * Prints information on the given keyval as log events.
	 * @param pId parallel Id of the print call.
	 * @param lId location Id of the print call.
	 * @param keyval to print.
	 * @return @see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN print (
    		MustParallelId pId,
    		MustLocationId lId,
    		MustKeyvalType keyval) = 0;

}; /*class I_PrintKeyval*/

#endif /*I_PRINTKEYVAL_H*/
