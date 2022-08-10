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
 * @file CollStratBinomial.h
 *       This module implements the broadcast, reduce and allreduce functions for the
 *       AllToneOne, OneToAll and AllToAll vector clock primitives using a binomial tree.
 *       Allreduce is implemented as a combination of reduce and broadcast.
 *
 * @author Felix Tomski
 * @date 11.09.2021
 */

#include "CollStratApi.h"
#include "GtiCollStrat.h"
#include "I_CollStrat.h"

#include "ModuleBase.h"
#include <unordered_map>
#include <vector>

#ifndef COLL_STRAT_NAIVE_H
#define COLL_STRAT_NAIVE_H

namespace gti {

class CollStratBinomial : public ModuleBase<CollStratBinomial, I_CollStrat>, public GtiCollStrat {
  private:
    binomialBcastSendP myBcastSend;
    binomialReduceSendP myReduceSend;
    binomialReduceSendP myAllreduceSend;

    /**
     * Forward the message upwards in the binomial tree to our parent.
     */
    GTI_ANALYSIS_RETURN propagate_broadcast(DType* buf, size_t count, int root, int myLocalRank,
                                           const int* groupRanks, size_t groupSize,
                                           GroupId groupId);

    /**
     * Performs a local reduction on the \p dest buffer. 
     */
    template <typename T>
    GTI_ANALYSIS_RETURN internal_reduce(const T* src, size_t src_count, T* dest, size_t dest_count,
                                        const std::vector<int>& groupRanks, int root,
                                        int localSrcRank, int localDstRank, GroupId gId);
    /**
     * Performs a local reduction on the \p dest buffer utilizing complement clocks.
     */
    template <typename T>
    GTI_ANALYSIS_RETURN reduce_complement(const T* src, size_t src_count, T* dest,
                                          size_t dest_count, const std::vector<int>& groupRanks,
                                          int originId, int localSrcRank, int localDstRank,
                                          GroupId gId);

    /**
     * Forward a reduce message to our parent in the binomial tree, and if necessary wait for
     * messages from our children.
     */
    GTI_ANALYSIS_RETURN propagate_reduce(DType* recvbuf,
                                size_t recv_count, size_t* numWaiting, int root, int myLocalRank,
                                const std::vector<int>& groupAppRanks, GroupId groupId,
                                size_t counter, ONGOING_COLL_TYPE collType);

    /**
     * Used for the reduce step of an allreduce. Here, each process only needs to send its
     * own entry and its entries for non-group children.
     */
    GTI_ANALYSIS_RETURN complement_reduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                          int root, int myLocalRank,
                                          const std::vector<int>& groupAppRanks, GroupId groupId);
    inline void placeModGuard() {
        if (!myPlaceMod)
            getPlaceMod(&myPlaceMod);
    }

    buf_t myInplaceReduceBuf; /* Temporal buffer used for inplace reduce */
    buf_t myInplaceAllreduceBuf; /* Temporal buffer used for inplace allreduce */
    DType myAllreduceMax;

  protected:
  public:
    /**
     * Constructor.
     * @ref ModConf - The module configuration syntax
     * @param instanceName name of this module instance.
     */
    CollStratBinomial(const char* instanceName);

    /**
     * Destructor.
     */
    ~CollStratBinomial(void);

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
}; /*class CollStratBinomial*/
} /*namespace gti*/

#endif /* COLL_STRAT_NAIVE_H */
