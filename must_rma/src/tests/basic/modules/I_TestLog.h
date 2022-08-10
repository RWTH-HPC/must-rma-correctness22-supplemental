/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_TestLocation.h
 *       @see I_TestLog.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"

#include "BaseIds.h"

#ifndef I_TESTLOG_H
#define I_TESTLOG_H

/**
 * Creates a logging message to test logging.
 */
class I_TestLog : public gti::I_Module
{
public:
	/**
	 * Creates a log event.
	 * @param pId parallel Id for the location
	 * @param lId location id to use for the message.
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN test (MustParallelId pId, MustLocationId lId) = 0;

}; /*class I_TestLog*/

#endif /*I_TESTLOG_H*/
