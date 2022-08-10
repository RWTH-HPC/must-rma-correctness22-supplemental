/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_InitLocationId.h
 *       Interface to set a location Id.
 *
 *  @date 24.04.2014
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "mustConfig.h"

#include "BaseIds.h"

#ifndef I_INITLOCATIONID_H
#define I_INITLOCATIONID_H

/**
 * Interface for an integrity analysis that can initialize a location ID.
 *
 * Dependencies:
 * - InitParallelId
 */
class I_InitLocationId : public gti::I_Module
{
public:
    /**
     * Sets the given storage to a location identifier for the current event context, e.g., just a call name
     * or a full call stack.
     * @param callName name of the function call that created the event.
     * @param callId a globally unified number for this call name.
     * @param pStorage pointer to the storage to which we will store the created parallel ID.
     * @return @see GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN init (MustLocationId *pStorage, const char* callName, int callId) = 0;
    virtual gti::GTI_ANALYSIS_RETURN getUniqueLocationId (MustLocationId *pStorage) = 0;

    /**
     * Generate a unique location identifier for a given code pointer.
     *
     *
     * @param pStorage   Pointer to the storage for the generated location ID.
     * @param codeptr_ra Pointer of the related code segment of this call.
     *
     * @return @see gti::GTI_ANALYSIS_RETURN
     */
    virtual gti::GTI_ANALYSIS_RETURN initCodePtr(const char* callName, MustLocationId* pStorage,
                                                 const void* codeptr_ra) = 0;
}; /*class I_InitLocationId*/

#endif /*I_INITLOCATIONID_H*/
