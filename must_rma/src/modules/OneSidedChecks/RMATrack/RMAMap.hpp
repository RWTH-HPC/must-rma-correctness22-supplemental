/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file RMAMap.hpp
 *       @see must::RMAMap.
 *
 *  @date 16.06.2017
 *  @author Simon Schwitanski
 */

//=============================
// addOp
//=============================
template < typename RMA_OP_TYPE >
void RMAMap<RMA_OP_TYPE>::addOp(MustRMAId id, RMA_OP_TYPE* op) {
    if (op) {
        myRMAMap.insert(std::make_pair(id, op));
        myWinRMAMap[op->getWinId()].insert(id);
    }
}

//=============================
// getOp
//=============================
template < typename RMA_OP_TYPE >
RMA_OP_TYPE * RMAMap<RMA_OP_TYPE>::getOp(MustRMAId callId) {
    typename std::map<MustRMAId, RMA_OP_TYPE*>::iterator it = myRMAMap.find(callId);
    if (it == myRMAMap.end())
        return NULL;
    else
        return it->second;
}

//=============================
// getWinOps
//=============================
template < typename RMA_OP_TYPE >
std::list<MustRMAId> RMAMap<RMA_OP_TYPE>::getWinOps(MustWinType win) {
    std::list<MustRMAId> ret;
    std::copy(myWinRMAMap[win].begin(), myWinRMAMap[win].end(), std::back_inserter(ret));

    return ret;
}

//=============================
// getWinTargetOps
//=============================
template < typename RMA_OP_TYPE >
std::list<MustRMAId> RMAMap<RMA_OP_TYPE>::getWinTargetOps(MustWinType win, int target) {
    std::list<MustRMAId> ret;

    for (std::set<MustRMAId>::iterator it = myWinRMAMap[win].begin();
            it != myWinRMAMap[win].end(); ++it) {
        RMA_OP_TYPE *op = getOp(*it);
        if (op->getTarget() == target)
            ret.push_back(*it);
    }

    return ret;
}

//=============================
// getWinOriginOps
//=============================
template < typename RMA_OP_TYPE >
std::list<MustRMAId> RMAMap<RMA_OP_TYPE>::getWinOriginOps(MustWinType win, int origin) {
    std::list<MustRMAId> ret;

    for (std::set<MustRMAId>::iterator it = myWinRMAMap[win].begin();
            it != myWinRMAMap[win].end(); ++it) {
        RMA_OP_TYPE *op = getOp(*it);
        if (op->getOrigin() == origin)
            ret.push_back(*it);
    }

    return ret;
}

//=============================
// removeOp 
//=============================
template < typename RMA_OP_TYPE >
void RMAMap<RMA_OP_TYPE>::removeOp(MustRMAId callId) {
    RMA_OP_TYPE* removeOp = getOp(callId);

    if (removeOp) {
        // remove from all containers
        myRMAMap.erase(callId); 
        myWinRMAMap[removeOp->getWinId()].erase(callId);
        // remove RMA op, it could be that there are user references such that
        // the object is not freed yet
        removeOp->mpiErase();
    }
}

//=============================
// removeOps
//=============================
template < typename RMA_OP_TYPE >
void RMAMap<RMA_OP_TYPE>::removeOps(const std::list<MustRMAId>& callIds) {
    for (std::list<MustRMAId>::const_iterator it = callIds.begin();
            it != callIds.end(); ++it) {
        removeOp(*it);
    }
}


//=============================
// size
//=============================
template < typename RMA_OP_TYPE >
size_t RMAMap<RMA_OP_TYPE>::size() {
    return myRMAMap.size();
}

