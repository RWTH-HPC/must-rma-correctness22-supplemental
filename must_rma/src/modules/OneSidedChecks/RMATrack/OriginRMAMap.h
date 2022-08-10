/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OriginRmaMap.h
 *
 *  @date 16.06.2017
 *  @author Simon Schwitanski
 */

#ifndef ORIGIN_RMA_MAP_H
#define ORIGIN_RMA_MAP_H

#include <map>

#include "RMAMap.h"
#include "OriginRMAOp.h"
#include "MustTypes.h"

namespace must {

/**
 * Implements a map that stores OriginRMAOp objects.
 * In addition to RMAMap, this map provides a method to access all memory
 * operations associated with a given request.
 */
class OriginRMAMap: public RMAMap<OriginRMAOp> {
    public:
        /**
         * @see must::RMAMap::addOp
         */
        virtual void addOp(MustRMAId id, OriginRMAOp* op);

        /**
         * @see must::RMAMap::removeOp
         */
        virtual void removeOp(MustRMAId callId);

        /**
         * Returns all stored OriginRMAOp objects that belong to a given request. 
         *
         * @param win win id the RMAOp objects are filtered by
         * @param origin origin rank 
         * @return list of RMAOp objects that belong to the given window and rank
         */
        virtual std::list<MustRMAId> getReqOps(MustRequestType req);
    private:
        // internal map for caching request-based RMA ops
        std::multimap<MustRequestType, MustRMAId> myReqRMAMap;
};

}

#endif
