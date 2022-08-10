/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_TSanMessages.h
 *       @see I_TSanMessages.
 *
 *  @date 23.11.2017
 *  @author Felix Dommes
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"

#include <string>

#ifndef I_TSANMESSAGES_H
#define I_TSANMESSAGES_H

/**
 * Interface for correctness checks of datatypes.
 *
 * Dependencies (order as listed):
 * - CreateMessage
 * - InitParallelId
 * - InitLocationId
 *
 */
class I_TSanMessages : public gti::I_Module
{
public:
    //Notify the module, that the analysis is shutting down.
	virtual gti::GTI_ANALYSIS_RETURN fini() = 0;
};/*class I_TSan_Messages*/

#endif /*I_TSANMESSAGES_H*/
