/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_OMPTIntegration.h
 *       @see gti::I_OMPTIntegration
 *
 * @author Tobias Hilbrich
 * @date 17.12.2009
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"

#ifdef GTI_OMPT_FOUND
#include <ompt.h>
#endif

#ifndef I_OMPTINTEGRATION_H
#define I_OMPTINTEGRATION_H

namespace gti
{
	class I_OMPTIntegration : public gti::I_Module
	{
	public:
#ifdef GTI_OMPT_FOUND
		virtual gti::GTI_ANALYSIS_RETURN getLookUpFunction( ompt_function_lookup_t *func ) = 0;
                virtual gti::GTI_ANALYSIS_RETURN setCallback( ompt_event_t, ompt_callback_t ) = 0;
                
                virtual void ompt_init_callback( ompt_function_lookup_t ompt_fn_lookup,
                            const char *runtime_version,
                            unsigned int ompt_version
                          ) = 0;
#endif
	};//I_OMPTIntegration
} //namespace gti

#endif //I_OMPTINTEGRATION_H