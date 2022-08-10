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
 * @file CollStratNaive.cpp
 *
 * @author Felix Tomski
 * @date 11.09.2021
 */

#include "CollStratNaive.h"
#include "GtiMacros.h"
#include <assert.h>

using namespace gti;

//=============================
// Basic Module functions
//=============================
mGET_INSTANCE_FUNCTION(CollStratNaive) mFREE_INSTANCE_FUNCTION(CollStratNaive)
    mPNMPI_REGISTRATIONPOINT_FUNCTION(CollStratNaive)

    //=============================
    // CollStratNaive
    //=============================
    CollStratNaive::CollStratNaive(const char* instanceName)
    : ModuleBase<CollStratNaive, I_CollStrat>(instanceName) {
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances();

    // Needs no sub modules
    assert(subModInstances.empty());

    auto data = getData();
    myNumProcs = stoi(data["gti_level_1_size"]);
    myComplementAllreduceBuf.reserve(myNumProcs - 1);
    myComplementAllreduceBuf.resize(myNumProcs - 1);

    auto& worldGroup = myGroups
                           .emplace(std::piecewise_construct, std::forward_as_tuple(0),
                                    std::forward_as_tuple(myNumProcs))
                           .first->second;
    for (int i = 0; i < myNumProcs; i++)
        worldGroup[i] = i;
    myCompGroups.emplace(std::piecewise_construct, std::forward_as_tuple(0),
                         std::forward_as_tuple(0));

    myIntraLayerTime = 0;

    getWrapAcrossFunction("naiveBcastSend", (GTI_Fct_t*)&myBcastSend);
    getWrapAcrossFunction("naiveReduceSend", (GTI_Fct_t*)&myReduceSend);
    getWrapAcrossFunction("naiveAllreduceSend", (GTI_Fct_t*)&myAllreduceSend);
}

//=============================
// ~CollStratNaive
//=============================
CollStratNaive::~CollStratNaive(void) {
    // nothing to do ...
}

//=============================
// broadcast
//=============================
GTI_ANALYSIS_RETURN CollStratNaive::broadcast(DType* buf, size_t count, int root, int myLocalRank,
                                              const std::vector<int>& groupAppRanks,
                                              GroupId groupId) {
    placeModGuard();
    if (myLocalRank != root) {
        auto bufferIt = myBcastBuffer.find(groupId);
        if (bufferIt == myBcastBuffer.end() || bufferIt->second.empty()) {
            /* Signal to recv function we're actively waiting for a broadcast message. */
            myInplaceData.set(buf, root, groupId, ONGOING_COLL_TYPE::BCAST);

            auto startTime = getUsecTime();
            while (myInplaceData.ptr)
                myPlaceMod->testIntralayer();
            myIntraLayerTime += getUsecTime() - startTime;

            myInplaceData.type = ONGOING_COLL_TYPE::NONE;
        } else {
            /* Otherwise we can use the first received message. */
            reduce_pairwise(bufferIt->second.front().data(), buf, count);
            bufferIt->second.pop();
        }
    } else {
        int memberId;
        for (int i = 0; i < groupAppRanks.size(); i++) {
            getLevelIdForApplicationRank(groupAppRanks[i], &memberId);
            if (i != root)
                myBcastSend(buf, count, nullptr, 0, i, root, groupId, myLocalRank, memberId);
        }
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// build_complement
//=============================
template <typename T>
GTI_ANALYSIS_RETURN CollStratNaive::build_complement(T* dest, const T* sendbuf, size_t count,
                                                     const std::vector<int>& groupRanks,
                                                     int myLocalRank, GroupId groupId) {
    dest[0] = sendbuf[groupRanks[myLocalRank]];
    if (groupRanks.size() < myNumProcs) {
        auto comp = findOrAddCompGroup(groupId, groupRanks);
        for (size_type i = 0; i < comp.size(); i++)
            dest[i + 1] = sendbuf[comp[i]];
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// reduce
//=============================
GTI_ANALYSIS_RETURN CollStratNaive::reduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                           int root, int myLocalRank,
                                           const std::vector<int>& groupAppRanks, GroupId groupId) {
    placeModGuard();
    auto isGroupNew = findOrAddGroup(groupId, groupAppRanks).second;
    auto counterIt = findOrAddReduceCounter(groupId).first;

    if (myLocalRank != root) {
        int memberId;
        getLevelIdForApplicationRank(groupAppRanks[root], &memberId);
        myReduceSend(sendbuf, myNumProcs, isGroupNew ? groupAppRanks.data() : nullptr,
                     isGroupNew ? groupAppRanks.size() : 0, root, root, counterIt->second, groupId,
                     myLocalRank, memberId);
    } else {
        auto bufferIt = myReduceBuffer.find(groupId);
        if (bufferIt == myReduceBuffer.end() || bufferIt->second.empty()) {
            myInplaceData.set(recvbuf, root, groupId, ONGOING_COLL_TYPE::REDUCE,
                              groupAppRanks.size() - 1);
            std::memcpy(myInplaceData.ptr, sendbuf, sizeof(DType) * count);

            auto startTime = getUsecTime();
            while (myInplaceData.numWaitingReduce > 0)
                myPlaceMod->testIntralayer();
            myIntraLayerTime += getUsecTime() - startTime;

            myInplaceData.type = ONGOING_COLL_TYPE::NONE;
        } else {
            auto startTime = getUsecTime();
            while (myReduceBuffer[groupId].front().first > 0)
                myPlaceMod->testIntralayer();
            myIntraLayerTime += getUsecTime() - startTime;

            reduce_both_full(sendbuf, myReduceBuffer[groupId].front().second.data(), count);
            std::memcpy(recvbuf, myReduceBuffer[groupId].front().second.data(),
                        sizeof(DType) * myNumProcs);

            myReduceBuffer[groupId].pop_front();
        }
    }

    counterIt->second += 1;

    return GTI_ANALYSIS_SUCCESS;
}
//=============================
// reduce
//=============================
GTI_ANALYSIS_RETURN CollStratNaive::complement_reduce(const DType* sendbuf, DType* recvbuf,
                                                      size_t count, int root, int myLocalRank,
                                                      const std::vector<int>& groupAppRanks,
                                                      GroupId groupId) {
    placeModGuard();
    auto isGroupNew = findOrAddGroup(groupId, groupAppRanks).second;
    auto counterIt = findOrAddAllreduceCounter(groupId).first;

    if (myLocalRank != root) {
        int memberId;
        build_complement(myComplementAllreduceBuf.data(), sendbuf, count, groupAppRanks,
                         myLocalRank, groupId);
        getLevelIdForApplicationRank(groupAppRanks[root], &memberId);
        myAllreduceSend(myComplementAllreduceBuf.data(), myNumProcs - groupAppRanks.size() + 1,
                        isGroupNew ? groupAppRanks.data() : nullptr,
                        isGroupNew ? groupAppRanks.size() : 0, root, root, counterIt->second,
                        groupId, myLocalRank, memberId);
    } else {
        auto bufferIt = myAllreduceBuffer.find(groupId);
        if (bufferIt == myAllreduceBuffer.end() || bufferIt->second.first == 0) {
            myInplaceData.set(recvbuf, root, groupId, ONGOING_COLL_TYPE::ALLREDUCE,
                              groupAppRanks.size() - 1);

            auto startTime = getUsecTime();
            while (myInplaceData.numWaitingReduce > 0)
                myPlaceMod->testIntralayer();
            myIntraLayerTime += getUsecTime() - startTime;

            internal_reduce(sendbuf, count, recvbuf, myNumProcs, groupAppRanks, myLocalRank,
                            groupId);
            myInplaceData.type = ONGOING_COLL_TYPE::NONE;
        } else {
            auto startTime = getUsecTime();
            while (myAllreduceBuffer[groupId].first > 0)
                myPlaceMod->testIntralayer();
            myIntraLayerTime += getUsecTime() - startTime;

            internal_reduce(sendbuf, count, myAllreduceBuffer[groupId].second.data(),
                            myAllreduceBuffer[groupId].second.size(), groupAppRanks, myLocalRank,
                            groupId);
            std::memcpy(recvbuf, myAllreduceBuffer[groupId].second.data(),
                        sizeof(DType) * myNumProcs);
            myAllreduceBuffer[groupId].first = 0;
        }
    }

    counterIt->second += 1;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// allreduce
//=============================
GTI_ANALYSIS_RETURN CollStratNaive::allreduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                              int myLocalRank,
                                              const std::vector<int>& groupAppRanks,
                                              GroupId groupId) {
    placeModGuard();
    complement_reduce(sendbuf, recvbuf, count, 0, myLocalRank, groupAppRanks, groupId);
    return broadcast(recvbuf, myNumProcs, 0, myLocalRank, groupAppRanks, groupId);
}

//=============================
// recvBcast
//=============================
GTI_ANALYSIS_RETURN CollStratNaive::recvBcast(DType* data, size_t count, int* groupRanks,
                                              size_t groupSize, int localTargetRank,
                                              int localRootRank, GroupId groupId,
                                              int localOriginRank) {
    /* Don't need to buffer if we're actively waiting. */
    if (myInplaceData.type == ONGOING_COLL_TYPE::BCAST && myInplaceData.gId == groupId &&
        myInplaceData.root == localRootRank) {
        reduce_pairwise(data, myInplaceData.ptr, count);
        myInplaceData.ptr = nullptr;
        return GTI_ANALYSIS_SUCCESS;
    }

    auto it = myBcastBuffer.find(groupId);
    if (it == myBcastBuffer.end())
        it = myBcastBuffer.emplace_hint(it, std::piecewise_construct,
                                        std::forward_as_tuple(groupId), std::forward_as_tuple());
    it->second.emplace(data, data + count);

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvReduce
//=============================
GTI_ANALYSIS_RETURN CollStratNaive::recvReduce(DType* data, size_t count, int* groupRanks,
                                               size_t groupSize, int localTargetRank,
                                               int localRootRank, size_t remoteCounter,
                                               GroupId groupId, int localOriginRank) {
    auto gIt = findOrAddGroup(groupId, groupRanks, groupRanks + groupSize);
    auto counterIt = findOrAddReduceCounter(groupId).first;

    /* Root itself (this process) already entered reduce and waits for clocks */
    if (myInplaceData.type == ONGOING_COLL_TYPE::REDUCE && counterIt->second == remoteCounter &&
        myInplaceData.gId == groupId && myInplaceData.root == localRootRank) {
        reduce_both_full(data, myInplaceData.ptr, count);
        myInplaceData.numWaitingReduce -= 1;
        return GTI_ANALYSIS_SUCCESS;
    }

    size_t counterDiff = remoteCounter - counterIt->second;
    if (myInplaceData.type == ONGOING_COLL_TYPE::REDUCE && myInplaceData.gId == groupId &&
        myInplaceData.root == localRootRank)
        counterDiff -= 1;

    /* Add reduce queue for this group if necessary */
    auto it = myReduceBuffer.find(groupId);
    if (it == myReduceBuffer.end())
        it = myReduceBuffer.emplace_hint(it, std::piecewise_construct,
                                         std::forward_as_tuple(groupId), std::forward_as_tuple());

    /* First clock that arrived for this reduction */
    if (it->second.empty() || counterDiff >= it->second.size())
        it->second.emplace_back(std::piecewise_construct,
                                std::forward_as_tuple(gIt.first->second.size() - 1),
                                std::forward_as_tuple(data, data + count));
    else
        reduce_both_full(data, it->second.at(counterDiff).second.data(), count);

    it->second.at(counterDiff).first -= 1;

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvAllreduce
//=============================
GTI_ANALYSIS_RETURN CollStratNaive::recvAllreduce(DType* data, size_t count, int* groupRanks,
                                                  size_t groupSize, int localTargetRank,
                                                  int localRootRank, size_t remoteCounter,
                                                  GroupId groupId, int localOriginRank) {
    auto gIt = findOrAddGroup(groupId, groupRanks, groupRanks + groupSize);
    auto counterIt = findOrAddAllreduceCounter(groupId).first;

    /* Directly merge the received message if it corresponds to an ongoing allreduce operation. */
    if (myInplaceData.type == ONGOING_COLL_TYPE::ALLREDUCE && myInplaceData.gId == groupId &&
        myInplaceData.root == localRootRank) {
        internal_reduce(data, count, myInplaceData.ptr, myNumProcs, gIt.first->second,
                        localOriginRank, groupId);
        myInplaceData.numWaitingReduce -= 1;
        return GTI_ANALYSIS_SUCCESS;
    }

    /* Else, see whether we already created the appropiate buffer, or create one if necessary. */
    auto it = myAllreduceBuffer.find(groupId);
    if (it == myAllreduceBuffer.end()) {
        it = myAllreduceBuffer.emplace_hint(
            it, std::piecewise_construct, std::forward_as_tuple(groupId), std::forward_as_tuple());
        it->second.first = 0;
    }

    if (it->second.first == 0)
        it->second.second.resize(myNumProcs);

    /* Merge message in the found or created buffer. */
    internal_reduce(data, count, it->second.second.data(),
                    it->second.second.size(), gIt.first->second, localOriginRank,
                    groupId);
    it->second.first -= 1;

    return GTI_ANALYSIS_SUCCESS;
}

template <typename T>
GTI_ANALYSIS_RETURN CollStratNaive::reduce_both_full(const T* src, T* dest, size_t count) {
    for (std::size_t i = 0; i < count; i++)
        if (src[i] > dest[i])
            dest[i] = src[i];

    return GTI_ANALYSIS_SUCCESS;
}

template <typename T>
GTI_ANALYSIS_RETURN CollStratNaive::reduce_complement(const T* src, size_t src_count, T* dest,
                                                      size_t dest_count, const AbstractBitset& bs,
                                                      size_t groupSize, int originId) {
    dest[originId] = src[0];
    for (std::size_t i = 0, j = 1; i < bs.bit_size(); i++)
        if (!bs[i] && dest[i] < src[j])
            dest[i] = src[j++];

    return GTI_ANALYSIS_SUCCESS;
}

template <typename T>
GTI_ANALYSIS_RETURN CollStratNaive::reduce_complement(const T* src, T* dest,
                                                      const std::vector<int>& compRanks,
                                                      int originId) {
    dest[originId] = src[0];
    for (std::size_t i = 0; i < compRanks.size(); i++)
        if (dest[compRanks[i]] < src[i + 1])
            dest[compRanks[i]] = src[i + 1];

    return GTI_ANALYSIS_SUCCESS;
}

template <typename T>
GTI_ANALYSIS_RETURN
CollStratNaive::internal_reduce(const T* src, size_t src_count, T* dest, size_t dest_count,
                                const std::vector<int>& groupRanks, int localSrcRank, GroupId gId) {
    int memberId;
    getLevelIdForApplicationRank(groupRanks[localSrcRank], &memberId);

    if (groupRanks.size() == myNumProcs)
        dest[memberId] = src[src_count == myNumProcs ? memberId : 0];
    else if (groupRanks.size() < myNumProcs) {
        auto compGroup = findOrAddCompGroup(gId, groupRanks);
        if (src_count == myNumProcs && dest_count == src_count)
            reduce_both_full(src, dest, src_count);
        else
            reduce_complement(src, dest, compGroup, memberId);
    }

    return GTI_ANALYSIS_SUCCESS;
}
