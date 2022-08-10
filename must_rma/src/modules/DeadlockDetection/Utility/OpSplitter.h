/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OpSplitter.h
 *       @see MUST::OpSplitter.
 *
 *  @date 05.04.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"

#include "I_OpSplitter.h"

#ifndef TEMPLATE_H
#define TEMPLATE_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_OpSplitter.
     */
    class OpSplitter : public gti::ModuleBase<OpSplitter, I_OpSplitter>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		OpSplitter (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~OpSplitter (void);

    		/**
    		 * @see I_OpSplitter::splitSendRecv.
    		 */
    		GTI_ANALYSIS_RETURN splitSendRecv (
    		        MustParallelId pId,
    		        MustLocationId lId,
    		        int dest,
    		        int sendtag,
    		        MustDatatypeType sendtype,
    		        int sendcount,
    		        int source,
    		        int recvtag,
    		        MustCommType recvtype,
    		        int recvcount,
    		        MustCommType comm);

    		/**
    		  * @see I_OpSplitter::splitStartall.
    		  */
    		GTI_ANALYSIS_RETURN splitStartall (
    		    		MustParallelId pId,
    		    		MustLocationId lId,
    		    		int count,
    		    		MustRequestType *requests);
    protected:
    };
} /*namespace MUST*/

#endif /*OPSPLITTER_H*/
