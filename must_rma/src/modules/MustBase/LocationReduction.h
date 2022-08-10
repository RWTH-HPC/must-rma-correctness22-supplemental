/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file LocationReduction.h
 *       @see LocationReduction
 *
 *  @date 11.01.2011
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_LocationReduction.h"
#include "I_LocationAnalysis.h"

#ifndef LOCATIONREDUCTION_H
#define LOCATIONREDUCTION_H

using namespace gti;

namespace must
{
	/**
	 * Class for a reduction that removes
	 * unnecessary location records.
	 * @see MUST::LocationImpl
	 */
	class LocationReduction : public gti::ModuleBase<LocationReduction, I_LocationReduction>
	{
	public:
		/**
		 * Constructor.
		 * @param instanceName name of this module instance.
		 */
		LocationReduction (const char* instanceName);

		/**
		 * Destructor.
		 */
		virtual ~LocationReduction (void);

		/**
		 * @see I_LocationReduction::reduce
		 */
		gti::GTI_ANALYSIS_RETURN reduce (
				MustParallelId pId,
				MustLocationId lId,
				char* callName,
				int callNameLen,
				const void* codeptr,
				const char* fname,
				size_t fnameLen,
				const void* fbase,
#ifdef ENABLE_STACKTRACE
				int numStackLevels,
				int stackInfosLength,
				int indicesLength,
				int* infoIndices,
				char* stackInfos,
#endif
				gti::I_ChannelId *thisChannel,
				std::list<gti::I_ChannelId*> *outFinishedChannels);

		/**
		 * @see I_Reduction::timeout.
		 */
		void timeout (void);

	protected:
		I_LocationAnalysis *myLocationModule;
	}; /*class LocationReduction*/
} /*namespace MUST*/

#endif /*LOCATIONREDUCTION_H*/
