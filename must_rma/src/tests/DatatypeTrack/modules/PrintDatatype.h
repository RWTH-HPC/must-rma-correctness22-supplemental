/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file PrintDatatype.h
 *       @see MUST::PrintDatatype.
 *
 *  @date 23.02.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_PrintDatatype.h"
#include "I_CreateMessage.h"

#ifndef PRINTDATATYPE_H
#define PRINTDATATYPE_H

using namespace gti;

namespace must
{
	/**
     * Implementation of I_PrintDatatype.
     */
    class PrintDatatype : public gti::ModuleBase<PrintDatatype, I_PrintDatatype>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
    		PrintDatatype (const char* instanceName);

    		/**
    		 * Destructor.
    		 */
    		virtual ~PrintDatatype (void);

    		/**
    		 * @see I_PrintDatatype::print.
    		 */
    		GTI_ANALYSIS_RETURN print (
    		    		MustParallelId pId,
    		    		MustLocationId lId,
    		    		MustDatatypeType type);

    protected:
    		I_DatatypeTrack* myTypes;
    		I_CreateMessage* myLog;

    }; /*class PrintDatatype */
} /*namespace MUST*/

#endif /*PRINTDATATYPE_H*/
