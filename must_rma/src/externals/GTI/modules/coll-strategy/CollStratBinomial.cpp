/* This file is part of GTI (Generic Tool Infrastructure)
 *
 * Copyright (C)
 *  2008-2019 ZIH, Technische Universitaet Dresden, Federal Republic of Germany
 *  2008-2019 Lawrence Livermore National Laboratories, United States of America
 *  2013-2019 RWTH Aachen University, Federal Republic of Germany
 *
 * See the LICENSE file in the package base directory for details
 */

/**
 * @file CollStratBinomial.cpp
 *
 * @author Felix Tomski
 * @date 11.09.2021
 */

#include "CollStratBinomial.h"
#include "GtiMacros.h"
#include <algorithm>
#include <assert.h>

#include "BinomialTree.h"

using namespace gti;
using namespace BinomialTree;

//=============================
// Basic Module functions
//=============================
mGET_INSTANCE_FUNCTION(CollStratBinomial) mFREE_INSTANCE_FUNCTION(CollStratBinomial)
    mPNMPI_REGISTRATIONPOINT_FUNCTION(CollStratBinomial)

    //=============================
    // CollStratBinomial
    //=============================
    CollStratBinomial::CollStratBinomial(const char* instanceName)
    : ModuleBase<CollStratBinomial, I_CollStrat>(instanceName), myInplaceReduceBuf(), myAllreduceMax(0) {
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances();

    // Needs no sub modules
    assert(subModInstances.empty());

    auto data = getData();
    myNumProcs = stoi(data["gti_level_1_size"]);
    myInplaceReduceBuf.reserve(myNumProcs);
    myInplaceReduceBuf.resize(myNumProcs);
    std::fill(myInplaceReduceBuf.begin(), myInplaceReduceBuf.end(), 0);

    myInplaceAllreduceBuf.reserve(myNumProcs);
    myInplaceAllreduceBuf.resize(myNumProcs);
    std::fill(myInplaceAllreduceBuf.begin(), myInplaceAllreduceBuf.end(), 0);

    auto& worldGroup = myGroups
                           .emplace(std::piecewise_construct, std::forward_as_tuple(0),
                                    std::forward_as_tuple(myNumProcs))
                           .first->second;
    for (int i = 0; i < myNumProcs; i++)
        worldGroup[i] = i;
    myCompGroups.emplace(std::piecewise_construct, std::forward_as_tuple(0),
                         std::forward_as_tuple(0));

    getWrapAcrossFunction("binomialBcastSend", (GTI_Fct_t*)&myBcastSend);
    getWrapAcrossFunction("binomialReduceSend", (GTI_Fct_t*)&myReduceSend);
    getWrapAcrossFunction("binomialAllreduceSend", (GTI_Fct_t*)&myAllreduceSend);

    myIntraLayerTime = 0;
}

//=============================
// ~CollStratBinomial
//=============================
CollStratBinomial::~CollStratBinomial(void) {
    // nothing to do ...
}

//=============================
// propagate_broadcast
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::propagate_broadcast(DType* buf, size_t count, int root,
                                                          int myLocalRank, const int* groupRanks,
                                                          size_t groupSize, GroupId groupId) {
    auto gIt = findOrAddGroup(groupId, groupRanks, groupRanks + groupSize);
    int memberId, tmpPow, localRemote;
    for (int k = std::ceil(std::log2(gIt.first->second.size())) - 1; k >= 0; k--) {
        tmpPow = 1 << k;
        if ((PERMUT_RANK(myLocalRank, root, gIt.first->second.size()) % (tmpPow << 1)) == 0 &&
            PERMUT_RANK(myLocalRank, root, gIt.first->second.size()) + tmpPow <
                gIt.first->second.size()) {
            localRemote = (myLocalRank + tmpPow) % gIt.first->second.size();
            getLevelIdForApplicationRank(gIt.first->second[localRemote], &memberId);
            myBcastSend(buf, count, gIt.second ? gIt.first->second.data() : nullptr,
                        gIt.second ? gIt.first->second.size() : 0, localRemote, root, groupId,
                        myLocalRank, memberId);
        }
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// broadcast
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::broadcast(DType* buf, size_t count, int root,
                                                 int myLocalRank,
                                                 const std::vector<int>& groupAppRanks,
                                                 GroupId groupId) {
    placeModGuard();

    /* Non-roots may need to wait before forwarding */
    if (myLocalRank != root) {
        auto bufferIt = myBcastBuffer.find(groupId);
        if (bufferIt == myBcastBuffer.end() || bufferIt->second.empty()) {
            myInplaceData.set(buf, root, groupId, ONGOING_COLL_TYPE::BCAST);

            /* Wait for the message from our direct child in the binomial tree. */
            while (myInplaceData.ptr)
                myPlaceMod->testIntralayer();

            myInplaceData.type = ONGOING_COLL_TYPE::NONE;
        /* In case the message from our child has already arrived, merge it.
         * Forwaring of the message to the parent in the tree happens directly in the receive function. */
        } else {
            reduce_pairwise(bufferIt->second.front().data(), buf, count);
            bufferIt->second.pop();
        }
        return GTI_ANALYSIS_SUCCESS;
    }

    return propagate_broadcast(buf, count, root, myLocalRank, groupAppRanks.data(),
                              groupAppRanks.size(), groupId);
}
//
//=============================
// reduce
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::propagate_reduce(DType* recvbuf, size_t recv_count,
                                               size_t* numWaiting, int root, int myLocalRank,
                                               const std::vector<int>& groupAppRanks,
                                               GroupId groupId, size_t counter,
                                               ONGOING_COLL_TYPE collType) {
    auto isGroupNew = findOrAddGroup(groupId, groupAppRanks).second || counter == 0;
    bool active = true;
    int memberId, tmpPow, localTarget;

    auto mySendFunc = (collType == ONGOING_COLL_TYPE::REDUCE) ? myReduceSend : myAllreduceSend;

    /* Wait until we recevived a message from each of our children in the binomial tree. */
    while (*numWaiting > 0)
        myPlaceMod->testIntralayer();

    /* Forward our computed reduce message part to our parent. */
    if (myLocalRank != root) {
        for (int k = 0; k < std::ceil(std::log2(groupAppRanks.size())); k++) {
            if (active) {
                tmpPow = 1 << k;
                if ((PERMUT_RANK(myLocalRank, root, groupAppRanks.size()) >> k) & 1) {
                    active = false;
                    localTarget =
                        (myLocalRank + groupAppRanks.size() - tmpPow) % groupAppRanks.size();
                    getLevelIdForApplicationRank(groupAppRanks[localTarget], &memberId);
                    mySendFunc(recvbuf, recv_count, isGroupNew ? groupAppRanks.data() : nullptr,
                               isGroupNew ? groupAppRanks.size() : 0, localTarget, root, counter,
                               groupId, myLocalRank, memberId);
                }
            }
        }
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// reduce
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::reduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                              int root, int myLocalRank,
                                              const std::vector<int>& groupAppRanks,
                                              GroupId groupId) {
    placeModGuard();
    auto counterIt = findOrAddReduceCounter(groupId).first;

    bool isInplace = false;

    auto bufferIt = myReduceBuffer.find(groupId);
    isInplace = (bufferIt == myReduceBuffer.end()) || bufferIt->second.empty();
    /* If no message of our children in the tree have arrived yet, merge tem directly in the output buffer. */
    if (isInplace) {
        myInplaceData.set(myLocalRank == root ? recvbuf : myInplaceReduceBuf.data(), root, groupId,
                          ONGOING_COLL_TYPE::REDUCE,
                          nodeHeight(groupAppRanks.size(), myLocalRank, root));

        /* Initalize the output buffer with the input. */
        std::memcpy(myInplaceData.ptr, sendbuf, sizeof(DType) * count);

        /* Wait for missing children and propagate our reduced part of the buffer up in the binomial tree. */
        propagate_reduce(myInplaceData.ptr, count, &(myInplaceData.numWaitingReduce), root, myLocalRank,
                groupAppRanks, groupId, counterIt->second, ONGOING_COLL_TYPE::REDUCE);
    } else {
        reduce_pairwise(sendbuf, bufferIt->second.front().second.data(), count);

        propagate_reduce(bufferIt->second.front().second.data(), bufferIt->second.front().second.size(),
                &(bufferIt->second.front().first), root, myLocalRank, groupAppRanks, groupId,
                counterIt->second, ONGOING_COLL_TYPE::REDUCE);

        if (myLocalRank == root)
            std::memcpy(recvbuf, bufferIt->second.front().second.data(), sizeof(DType) * count);
        bufferIt->second.pop_front();
    }

    if (isInplace)
        myInplaceData.type = ONGOING_COLL_TYPE::NONE;
    counterIt->second += 1;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// complement_reduce
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::complement_reduce(const DType* sendbuf, DType* recvbuf,
                                                         size_t count, int root, int myLocalRank,
                                                         const std::vector<int>& groupAppRanks,
                                                         GroupId groupId) {
    placeModGuard();
    auto counterIt = findOrAddAllreduceCounter(groupId).first;

    bool isInplace = false;
    size_t countRef = count;

    auto bufferIt = myAllreduceBuffer.find(groupId);
    isInplace = (bufferIt == myAllreduceBuffer.end()) || bufferIt->second.first == INVALID_ALLREDUCE_BUF;
    if (isInplace) {
        if (myLocalRank == root) {
            myInplaceData.set(recvbuf, root, groupId, ONGOING_COLL_TYPE::ALLREDUCE,
                              nodeHeight(groupAppRanks.size(), myLocalRank, root));
            countRef = myNumProcs;
        } else {
            /* Non-root processes only propagate messages of size: 1+ #children + #non-group */
            myInplaceAllreduceBuf.resize(
                nodeClockSize(myNumProcs, groupAppRanks.size(), myLocalRank, root));
            myInplaceData.set(myInplaceAllreduceBuf.data(), root, groupId,
                              ONGOING_COLL_TYPE::ALLREDUCE,
                              nodeHeight(groupAppRanks.size(), myLocalRank, root));
            countRef = myInplaceAllreduceBuf.size();
        }
        internal_reduce(sendbuf, count, myInplaceData.ptr, countRef, groupAppRanks, root,
                        myLocalRank, myLocalRank, groupId);

        propagate_reduce(myInplaceData.ptr, countRef, &(myInplaceData.numWaitingReduce), root, myLocalRank,
                groupAppRanks, groupId, counterIt->second, ONGOING_COLL_TYPE::ALLREDUCE);
        myInplaceData.type = ONGOING_COLL_TYPE::NONE;
    } else {
        internal_reduce(sendbuf, count, bufferIt->second.second.data(),
                        bufferIt->second.second.size(), groupAppRanks, root, myLocalRank,
                        myLocalRank, groupId);
        propagate_reduce(bufferIt->second.second.data(), bufferIt->second.second.size(),
                &(bufferIt->second.first), root, myLocalRank, groupAppRanks, groupId,
                counterIt->second, ONGOING_COLL_TYPE::ALLREDUCE);

        if (myLocalRank == root)
            std::memcpy(recvbuf, bufferIt->second.second.data(), sizeof(DType) * count);
        bufferIt->second.first = INVALID_ALLREDUCE_BUF;
    }

    counterIt->second += 1;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// allreduce
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::allreduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                                 int myLocalRank,
                                                 const std::vector<int>& groupAppRanks,
                                                 GroupId groupId) {
    GTI_ANALYSIS_RETURN ret;
    placeModGuard();

    complement_reduce(sendbuf, recvbuf, count, 0, myLocalRank, groupAppRanks, groupId);
    return broadcast(recvbuf, count, 0, myLocalRank, groupAppRanks, groupId);
//
//    if (myLocalRank == 0)
//        myAllreduceMax = *std::max_element(recvbuf, recvbuf + count);
//    ret = broadcast(&myAllreduceMax, 1, 0, myLocalRank, groupAppRanks, groupId);
//
//    std::fill(recvbuf, recvbuf + count, myAllreduceMax);
//
//    return ret;
}

//=============================
// recvBcast
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::recvBcast(DType* data, size_t count, int* groupRanks,
                                                 size_t groupSize, int localTargetRank,
                                                 int localRootRank, GroupId groupId,
                                                 int localOriginRank) {
    placeModGuard();

    propagate_broadcast(data, count, localRootRank, localTargetRank, groupRanks, groupSize, groupId);

    /* In case the broadcast is currently ongoing, merge our output buffer directly. */
    if (myInplaceData.type == ONGOING_COLL_TYPE::BCAST && myInplaceData.gId == groupId &&
        myInplaceData.root == localRootRank) {
        reduce_pairwise(data, myInplaceData.ptr, count);
        myInplaceData.ptr = nullptr;
    } else {
        auto bufferIt = myBcastBuffer.find(groupId);
        if (bufferIt == myBcastBuffer.end())
            bufferIt =
                myBcastBuffer.emplace_hint(bufferIt, std::piecewise_construct,
                                           std::forward_as_tuple(groupId), std::forward_as_tuple());
        bufferIt->second.emplace(data, data + count);
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvReduce
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::recvReduce(DType* data, size_t count, int* groupRanks,
                                                  size_t groupSize, int localTargetRank,
                                                  int localRootRank, size_t remoteCounter,
                                                  GroupId groupId, int localOriginRank) {
    placeModGuard();

    auto counterIt = findOrAddReduceCounter(groupId).first;
    auto gIt = findOrAddGroup(groupId, groupRanks, groupRanks + groupSize);

    /* In case the reduce is currently ongoing, perform an in-place reduction. */
    if (myInplaceData.type == ONGOING_COLL_TYPE::REDUCE && counterIt->second == remoteCounter &&
        myInplaceData.gId == groupId && myInplaceData.root == localRootRank) {
        reduce_pairwise(data, myInplaceData.ptr, count);
        myInplaceData.numWaitingReduce -= 1;
        return GTI_ANALYSIS_SUCCESS;
    }

    /* Compute the right position in the reduce queue. (Multiple reductions may be ongoing
     * concurrently without this process entered any.) */
    size_t counterDiff = remoteCounter - counterIt->second;
    if (myInplaceData.type == ONGOING_COLL_TYPE::REDUCE && myInplaceData.gId == groupId &&
        myInplaceData.root == localRootRank && counterDiff > 0)
        counterDiff -= 1;

    auto it = myReduceBuffer.find(groupId);
    if (it == myReduceBuffer.end())
        it = myReduceBuffer.emplace_hint(it, std::piecewise_construct,
                                         std::forward_as_tuple(groupId), std::forward_as_tuple());

    if (it->second.empty() || counterDiff + 1 > it->second.size())
        it->second.emplace_back(std::piecewise_construct,
                                std::forward_as_tuple(nodeHeight(gIt.first->second.size(),
                                                                 localTargetRank, localRootRank)),
                                std::forward_as_tuple(data, data + count));
    else
        reduce_pairwise(data, it->second.at(counterDiff).second.data(), count);

    it->second.at(counterDiff).first -= 1;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvAllreduce
//=============================
GTI_ANALYSIS_RETURN CollStratBinomial::recvAllreduce(DType* data, size_t count, int* groupRanks,
                                                     size_t groupSize, int localTargetRank,
                                                     int localRootRank, size_t remoteCounter,
                                                     GroupId groupId, int localOriginRank) {
    placeModGuard();

    auto gIt = findOrAddGroup(groupId, groupRanks, groupRanks + groupSize);

    /* In case the allreduce is currently ongoing, perform an in-place reduction. */
    if (myInplaceData.type == ONGOING_COLL_TYPE::ALLREDUCE &&
        myInplaceData.gId == groupId && myInplaceData.root == localRootRank) {
        internal_reduce(data, count, myInplaceData.ptr, myNumProcs, gIt.first->second,
                        localRootRank, localOriginRank, localTargetRank, groupId);
        myInplaceData.numWaitingReduce -= 1;
        return GTI_ANALYSIS_SUCCESS;
    }

    /* Otherwise, create the necessary buffer of appropiate size. */
    auto it = myAllreduceBuffer.find(groupId);
    if (it == myAllreduceBuffer.end()) {
        it = myAllreduceBuffer.emplace_hint(
            it, std::piecewise_construct, std::forward_as_tuple(groupId), std::forward_as_tuple());
        it->second.first = INVALID_ALLREDUCE_BUF;
    }
    if (it->second.first == INVALID_ALLREDUCE_BUF) {
        it->second.second.resize(
            nodeClockSize(myNumProcs, gIt.first->second.size(), localTargetRank, localRootRank));
        it->second.first = nodeHeight(gIt.first->second.size(), localTargetRank, localRootRank);
    }

    /* An merge the received message into the buffer. */
    internal_reduce(data, count, it->second.second.data(), it->second.second.size(),
                    gIt.first->second, localRootRank, localOriginRank, localTargetRank, groupId);
    it->second.first -= 1;

    return GTI_ANALYSIS_SUCCESS;
}

template <typename T>
GTI_ANALYSIS_RETURN
CollStratBinomial::reduce_complement(const T* src, size_t src_count, T* dest, size_t dest_count,
                                     const std::vector<int>& groupRanks, int root, int localSrcRank,
                                     int localDstRank, GroupId gId) {
    std::size_t compSize = myNumProcs - groupRanks.size();
    std::size_t destCompOffset = dest_count - compSize, srcCompOffset = src_count - compSize;
    int memberId;
    getLevelIdForApplicationRank(groupRanks[localDstRank], &memberId);
    /* Merge binomial reduction clock with own full clock */
    if (localSrcRank == localDstRank && src_count == myNumProcs) {
        dest[0] = src[memberId];

        if (groupRanks.size() < myNumProcs) {
            auto compgroup = findOrAddCompGroup(gId, groupRanks);
            for (size_type i = 0; i < compgroup.size(); i++)
                dest[i + 1] = src[compgroup[i]];
        }

        return GTI_ANALYSIS_SUCCESS;
    }
    /* Root merging remote reduction clock */
    else if (root == localDstRank && localDstRank != localSrcRank) {
        /* Group entries */
        for (size_type i = 0; i < srcCompOffset; i++) {
            getLevelIdForApplicationRank(groupRanks[(localSrcRank + i) % groupRanks.size()],
                                         &memberId);
            dest[memberId] = src[i];
        }
        /* Complement group entries */
        if (groupRanks.size() < myNumProcs) {
            const auto& compgroup = findOrAddCompGroup(gId, groupRanks);
            for (std::size_t i = 0; i < compgroup.size(); i++)
                if (dest[compgroup[i]] < src[i + srcCompOffset])
                    dest[compgroup[i]] = src[i + srcCompOffset];
        }
        return GTI_ANALYSIS_SUCCESS;
    }
    /* Merge binomial reduction clock with remote binomial reduction clock */
    else if (localSrcRank != localDstRank)
        std::memcpy(dest + posForClockSize(groupRanks.size(), localDstRank, localSrcRank, root),
                    src, sizeof(DType) * srcCompOffset);
    /* Merge binomial reduction clock with own complement clock */
    else if (localSrcRank == localDstRank)
        dest[0] = src[0];

    /* Update complement entries */
    if (compSize > 0)
        reduce_pairwise(src + srcCompOffset, dest + destCompOffset, compSize);

    return GTI_ANALYSIS_SUCCESS;
}

template <typename T>
GTI_ANALYSIS_RETURN
CollStratBinomial::internal_reduce(const T* src, size_t src_count, T* dest, size_t dest_count,
                                   const std::vector<int>& groupRanks, int root, int localSrcRank,
                                   int localDstRank, GroupId gId) {
    if (groupRanks.size() == myNumProcs) {
        if (src_count == dest_count) {
            int memberId;
            getLevelIdForApplicationRank(groupRanks[localSrcRank], &memberId);
            dest[memberId] = src[src_count == myNumProcs ? memberId : 0];
        } else
            reduce_complement(src, src_count, dest, dest_count, groupRanks, root, localSrcRank,
                              localDstRank, gId);
    } else if (groupRanks.size() < myNumProcs) {
        if (src_count == myNumProcs && dest_count == src_count)
            reduce_pairwise(src, dest, src_count);
        else
            reduce_complement(src, src_count, dest, dest_count, groupRanks, root, localSrcRank,
                              localDstRank, gId);
    }

    return GTI_ANALYSIS_SUCCESS;
}
