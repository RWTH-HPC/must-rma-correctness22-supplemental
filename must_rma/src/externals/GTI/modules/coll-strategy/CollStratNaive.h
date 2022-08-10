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
 * @file CollStratNaive.h
 *       This module implements the broadcast, reduce and allreduce functions for the
 *       AllToneOne, OneToAll and AllToAll vector clock primitives using a flat tree.
 *       Allreduce is implemented as a combination of reduce and broadcast.
 *
 * @author Felix Tomski
 * @date 11.09.2021
 */

#include "CollStratApi.h"
#include "GtiCollStrat.h"
#include "I_CollStrat.h"
#include "ModuleBase.h"

#include <deque>
#include <queue>
#include <unordered_map>
#include <vector>

#ifndef COLL_STRAT_NAIVE_H
#define COLL_STRAT_NAIVE_H

namespace gti {

class CollStratNaive : public ModuleBase<CollStratNaive, I_CollStrat>, public GtiCollStrat {
  private:
    buf_t myComplementAllreduceBuf;

    naiveBcastSendP myBcastSend;
    naiveReduceSendP myReduceSend;
    naiveAllreduceSendP myAllreduceSend;

    inline void placeModGuard() {
        if (!myPlaceMod)
            getPlaceMod(&myPlaceMod);
    }

  protected:
    template <typename T> GTI_ANALYSIS_RETURN reduce_both_full(const T* src, T* dest, size_t count);

    template <typename T>
    GTI_ANALYSIS_RETURN reduce_complement(const T* src, size_t src_count, T* dest,
                                          size_t dest_count, const AbstractBitset& bs,
                                          size_t groupSize, int originId);
    template <typename T>
    GTI_ANALYSIS_RETURN reduce_complement(const T* src, T* dest, const std::vector<int>& compRanks,
                                          int originId);
    template <typename T>
    GTI_ANALYSIS_RETURN internal_reduce(const T* src, size_t src_count, T* dest, size_t dest_count,
                                        const std::vector<int>& groupRanks, int localOwnerRank,
                                        GroupId gId);

    /* Creates a complement clock (first entry is the own entry, following all entries for processes not in the group) 
     * for the flat tree topology. */
    template <typename T>
    GTI_ANALYSIS_RETURN build_complement(T* dest, const T* sendbuf, size_t count,
                                         const std::vector<int>& groupRanks, int myLocalRank,
                                         GroupId groupId);

  public:
    /**
     * Constructor.
     * @ref ModConf - The module configuration syntax
     * @param instanceName name of this module instance.
     */
    CollStratNaive(const char* instanceName);

    /**
     * Destructor.
     */
    ~CollStratNaive(void);

    /**
     * @see gti::I_CollStrat::broadcast
     */
    GTI_ANALYSIS_RETURN broadcast(DType* buf, size_t count, int root, int myLocalRank,
                                  const std::vector<int>& groupRanks, GroupId groupId);

    /**
     * @see gti::I_CollStrat::reduce
     */
    GTI_ANALYSIS_RETURN reduce(const DType* sendbuf, DType* recvbuf, size_t count, int root,
                               int myLocalRank, const std::vector<int>& groupAppRanks,
                               GroupId groupId);

    /**
     * @see gti::I_CollStratBinomial::complement_reduce
     */
    GTI_ANALYSIS_RETURN complement_reduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                          int root, int myLocalRank,
                                          const std::vector<int>& groupAppRanks, GroupId groupId);

    /**
     * @see gti::I_CollStrat::allreduce
     */
    GTI_ANALYSIS_RETURN allreduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                  int myLocalRank, const std::vector<int>& groupAppRanks,
                                  GroupId groupId);

    /**
     * @see gti::I_CollStrat::recvBcast
     */
    GTI_ANALYSIS_RETURN recvBcast(DType* data, size_t count, int* groupRanks, size_t groupSize,
                                  int localTargetRank, int localRootRank, GroupId groupId,
                                  int localOriginRank);

    /**
     * @see gti::I_CollStrat::recvReduce
     */
    GTI_ANALYSIS_RETURN recvReduce(DType* data, size_t count, int* groupRanks, size_t groupSize,
                                   int localTargetRank, int localRootRank, size_t remoteCounter,
                                   GroupId groupId, int localOriginRank);

    /**
     * @see gti::I_CollStrat::recvAllreduce
     */
    GTI_ANALYSIS_RETURN recvAllreduce(DType* data, size_t count, int* groupRanks, size_t groupSize,
                                      int localTargetRank, int localRootRank, size_t remoteCounter,
                                      GroupId groupId, int localOriginRank);
    uint64_t getCollIntraLayerTime() const { return myIntraLayerTime; };
}; /*class CollStratNaive*/
} /*namespace gti*/

#endif /* COLL_STRAT_NAIVE_H */
