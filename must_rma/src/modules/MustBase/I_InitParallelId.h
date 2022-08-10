/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_InitParallelId.h
 *       Interface to set a parallel Id.
 *
 *  @date 16.04.2014
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"

#include "BaseIds.h"

#ifndef I_INITPARALLELID_H
#define I_INITPARALLELID_H

/**
 * Interface for an integrity analysis that can initialize a parallel ID.
 */
class I_InitParallelId : public gti::I_Module
{
public:
    /**
     * Sets the given storage to a parallel identifier for this application process/thread/?.
     * (What influences the parallel ID depends on what paradigm we are talking about)
     * @param pStorage pointer to the storage to which we will store the created parallel ID.
     * @return @see GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN init (MustParallelId *pStorage) = 0;

}; /*class I_InitParallelId*/

#endif /*I_INITPARALLELID_H*/
