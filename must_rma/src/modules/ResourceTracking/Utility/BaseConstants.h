/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file BaseConstants.h
 *       @see MUST::BaseConstants.
 *
 *  @date 12.04.2011
 *  @author Mathias Korepkat
 */

#include "ModuleBase.h"
#include "I_BaseConstants.h"

#include <map>
#include <atomic>

#ifndef BASECONSTANTS_H
#define BASECONSTANTS_H

using namespace gti;

namespace must
{
	/**
	 * Implementation of I_BaseConstants.
	 */
	class BaseConstants : public gti::ModuleBase<BaseConstants, I_BaseConstants, false>
	{
	public:
		/**
		 * Constructor.
		 * @param instanceName name of this module instance.
		 */
		BaseConstants (const char* instanceName);

		/**
		 * Destructor.
		 */
		~BaseConstants (void);

		/**
		 * @see I_BaseConstants::addConstants.
		 */
		GTI_ANALYSIS_RETURN addConstants (
				int mpiProcNull,
				int mpiAnySource,
				int mpiAnyTag,
				int mpiUndefined,
				int mpiBsendOverhead,
				int mpiTagUb,
				int mpiVersion,
				int mpiSubversion,
                int mpiDistributeBlock,
                int mpiDistributeCyclic,
                int mpiDistributeNone,
                int mpiDistributeDfltDarg,
                int mpiOrderC,
                int mpiOrderFortran,
				void* mpiBottom
				);

		 /**
		 * @see I_BaseConstants::isProcNull.
		 */
		 bool isProcNull (int val);

		 /**
		 * @see I_BaseConstants::getProcNull.
		 */
		 int getProcNull (void);

		 /**
		 * @see I_BaseConstants::isAnySource
		 */
		 bool isAnySource (int val);

		 /**
		 * @see I_BaseConstants::getAnySource.
		 */
		 int getAnySource (void);

		 /**
		 * @see I_BaseConstants::isAnyTag.
		 */
		 bool isAnyTag (int val);

		 /**
		 * @see I_BaseConstants::getAnyTag.
		 */
		 int getAnyTag (void);

		 /**
		 * @see I_BaseConstants::isUndefined.
		 */
		 bool isUndefined (int val);

		 /**
		 * @see I_BaseConstants::getUndefined.
		 */
		 int getUndefined (void);

		 /**
		 * @see I_BaseConstants::isBsendOverhead.
		 */
		 bool isBsendOverhead (int val);

		 /**
		 * @see I_BaseConstants::getBsendOverhead.
		 */
		 int getBsendOverhead (void);

		 /**
		 * @see I_BaseConstants::isTagUb.
		 */
		 bool isTagUb (int val);

		 /**
		 * @see I_BaseConstants::getTagUb.
		 */
		 int getTagUb (void);

		 /**
		 * @see I_BaseConstants::isVersion.
		 */
		 bool isVersion (int val);

		 /**
		 * @see I_BaseConstants::getVersion.
		 */
		 int getVersion (void);

		 /**
		 * @see I_BaseConstants::isSubversion.
		 */
		 bool isSubversion (int val);

		 /**
		 * @see I_BaseConstants::getSubversion.
		 */
		 int getSubversion (void);

        /**
         * @see I_BaseConstants::isDistributeBlock.
         */
        bool isDistributeBlock (int val);

        /**
         * @see I_BaseConstants::getDistributeBlock.
         */
        int getDistributeBlock (void);

        /**
         * @see I_BaseConstants::isDistributeCyclic.
         */
        bool isDistributeCyclic (int val);

        /**
         * @see I_BaseConstants::getDistributeCyclic.
         */
        int getDistributeCyclic (void);

        /**
         * @see I_BaseConstants::isDistributeNone.
         */
        bool isDistributeNone (int val);

        /**
         * @see I_BaseConstants::getDistributeNone.
         */
        int getDistributeNone (void);

        /**
         * @see I_BaseConstants::isDistributeDfltDarg.
         */
        bool isDistributeDfltDarg (int val);

        /**
         * @see I_BaseConstants::getDistributeDfltDarg.
         */
        int getDistributeDfltDarg (void);

        /**
         * @see I_BaseConstants::isOrderC.
         */
        bool isOrderC (int val);

        /**
         * @see I_BaseConstants::getOrderC.
         */
        int getOrderC (void);

        /**
         * @see I_BaseConstants::isOrderFortran.
         */
        bool isOrderFortran (int val);

        /**
         * @see I_BaseConstants::getOrderFortran.
         */
        int getOrderFortran (void);

        /**
		 * @see I_BaseConstants::isBottom.
		 */
		bool isBottom (void* val);

		/**
		 * @see I_BaseConstants::getBottom.
		 */
		void* getBottom (void);

	protected:
          std::atomic<int> myMpiProcNull{-1};
          std::atomic<int> myMpiAnySource{-1}, myMpiAnyTag{-1}, myMpiTagUb{-1};
          std::atomic<int> myMpiUndefined{-1};
          std::atomic<int> myMpiBsendOverhead{-1};
          std::atomic<int> myMpiVersion{-1}, myMpiSubversion{-1};
          std::atomic<int> myMpiDistributeBlock{-1}, myMpiDistributeCyclic{-1},
              myMpiDistributeNone{-1}, myMpiDistributeDfltDarg{-1};
          std::atomic<int> myMpiOrderC{-1}, myMpiOrderFortran{-1};
          std::atomic<void *> myMpiBottom{nullptr};
        };

} //end namespace MUST

#endif /*BASECONSTANTS_H*/
