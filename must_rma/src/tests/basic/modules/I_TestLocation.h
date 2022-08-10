/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_TestLocation.h
 *       @see I_TestLocation.
 *
 *  @date 10.01.2011
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"

#include "BaseIds.h"

#ifndef I_TESTLOCATION_H
#define I_TESTLOCATION_H

/**
 * Prints location and parallel id information.
 */
class I_TestLocation : public gti::I_Module
{
public:
	/**
	 * Prints location and parallel id.
	 * @param pId parallel Id for the location
	 * @param lId location id to print.
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN print (MustParallelId pId, MustLocationId lId) = 0;

}; /*class I_TestLocation*/

#endif /*I_TESTLOCATION_H*/
