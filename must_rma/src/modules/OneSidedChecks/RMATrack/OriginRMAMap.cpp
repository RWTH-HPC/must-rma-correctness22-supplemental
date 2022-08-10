/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OriginRMAMap.cpp
 *       @see must::RMAMAp.
 *
 *  @date 16.06.2017
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"
#include "OriginRMAMap.h"

using namespace must;

//=============================
// addOp
//=============================
void OriginRMAMap::addOp(MustRMAId id, OriginRMAOp* op) {
    RMAMap::addOp(id, op);

    // if RMA op is request-based, we add it to the corresponding map
    // to track its status
    // Note: An RMA origin operation completes either by an origin
    // completion function or in case of request-based RMA operations
    // by request completion.
    if (op && op->getRequestId() != 0) {
        myReqRMAMap.insert(std::make_pair(op->getRequestId(), id));
    }
}

//=============================
// getReqOps
//=============================
std::list<MustRMAId> OriginRMAMap::getReqOps(MustRequestType req) {
    std::list<MustRMAId> ret;

    // iterate over all memory operations associated with this request
    // (could be several e.g. in case of Rget_accumulate call which is split
    // in two independent calls)
    std::pair <std::multimap<MustRequestType, MustRMAId>::iterator,
               std::multimap<MustRequestType, MustRMAId>::iterator> range;
    range = myReqRMAMap.equal_range(req);

    for (std::multimap<MustRequestType, MustRMAId>::iterator it = range.first; it != range.second; ++it) {
        ret.push_back(it->second);
    }

    return ret;
}

//=============================
// removeOp
//=============================
void OriginRMAMap::removeOp(MustRMAId callId) {
    const OriginRMAOp* removeOp = getOp(callId);

    if (removeOp) {
        // delete from request map
        std::pair <std::multimap<MustRequestType, MustRMAId>::iterator,
               std::multimap<MustRequestType, MustRMAId>::iterator> range;
        // look up corresponding request
        range = myReqRMAMap.equal_range(removeOp->getRequestId());
        for(std::multimap<MustRequestType, MustRMAId>::iterator it = range.first;
                it != range.second; ++it) {
            // remove call (there might be multiple calls associated with that request)
            if (it->second == callId) {
                myReqRMAMap.erase(it);
                break;
            }
        }

        // finally delete from the other containers
        RMAMap::removeOp(callId);
    }
}
