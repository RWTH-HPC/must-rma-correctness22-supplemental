/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file BufferChecks.h
 *       @see MUST::BufferChecks.
 *
 *  @date 11.01.2013
 *  @author Joachim Protze
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"
#include "I_ArgumentAnalysis.h"
#include "I_CreateMessage.h"

#include "I_BufferChecks.h"

#include <string>

#ifndef BUFFERCHECKS_H
#define BUFFERCHECKS_H

using namespace gti;

namespace must
{
    /**
     * BufferChecks for correctness checks interface implementation.
     */
    class BufferChecks : public gti::ModuleBase<BufferChecks, I_BufferChecks>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
            BufferChecks (const char* instanceName);

            /**
             * Destructor.
             */
            virtual ~BufferChecks (void);

            /**
             * @see I_BufferChecks::bufferAttach.
             */
    GTI_ANALYSIS_RETURN bufferAttach (
            MustParallelId pId, 
            MustLocationId lId, 
            int aId, 
            int size);

            /**
             * @see I_BufferChecks::bufferDetach.
             */
    GTI_ANALYSIS_RETURN bufferDetach (
            MustParallelId pId, 
            MustLocationId lId);

            /**
             * @see I_BufferChecks::bufferUsage.
             */
    GTI_ANALYSIS_RETURN bufferUsage (
            MustParallelId pId, 
            MustLocationId lId,
            int size);

    protected:
            I_ParallelIdAnalysis* myPIdMod;
            I_CreateMessage* myLogger;
            I_ArgumentAnalysis* myArgMod;
            
            int bufferSize, bufferLoad;
    };
} /*namespace MUST*/

#endif /*BUFFERCHECKS_H*/
