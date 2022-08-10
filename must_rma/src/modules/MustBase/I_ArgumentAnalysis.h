/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_ArgumentAnalysis.h
 *       @see I_ArgumentAnalysis.
 *
 *  @date 28.02.2011
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"

#include "BaseIds.h"

#ifndef I_ARGUMENTANALYSIS_H
#define I_ARGUMENTANALYSIS_H

/**
 * Interface for translating argument ids into an argument index and
 * an argument name.
 */
class I_ArgumentAnalysis : public gti::I_Module
{
public:
	/**
	 * Returns the index of the argument in its MPI call.
	 * Starts with 1 for the first argument not with 0.
	 * @param id to query for information.
	 * @return index of argument.
	 */
    virtual int getIndex (MustArgumentId id) = 0;

    /**
     * Returns the name of an MPI calls argument.
     * @param id to query for information.
     * @return name of argument.
     */
    virtual std::string getArgName (MustArgumentId id) = 0;

}; /*class I_ArgumentAnalysis*/

#endif /*I_ARGUMENTANALYSIS_H*/
