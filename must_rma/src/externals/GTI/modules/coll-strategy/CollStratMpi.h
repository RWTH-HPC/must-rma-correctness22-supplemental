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
 * @file CollStratMpi.h
 *       This module implements the broadcast, reduce and allreduce functions for the
 *       AllToneOne, OneToAll and AllToAll vector clock primitives using the respective MPI functions.
 *
 * @author Felix Tomski
 * @date 11.09.2021
 */

#include "CollStratApi.h"
#include "I_CollStrat.h"
#include "ModuleBase.h"

#include <unordered_map>
#include <vector>

#ifndef COLL_STRAT_NAIVE_H
#define COLL_STRAT_NAIVE_H

namespace gti {

class CollStratMpi : public ModuleBase<CollStratMpi, I_CollStrat> {
  private:
    std::unordered_map<GroupId, MPI_Comm> myCommunicators;
    auto findOrAddComm(GroupId id, const std::vector<int>& ranks)
        -> decltype(myCommunicators.find(id)->second);

    MPI_Comm myToolComm;
    MPI_Group myToolGroup;
    MPI_Group myWorldGroup;

    std::vector<DType> myBcastBuffer;

  protected:
  public:
    /**
     * Constructor.
     * @ref ModConf - The module configuration syntax
     * @param instanceName name of this module instance.
     */
    CollStratMpi(const char* instanceName);

    /**
     * Destructor.
     */
    ~CollStratMpi(void);

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
                                  int localOriginRank) {
        return GTI_ANALYSIS_SUCCESS;
    };

    /**
     * @see gti::I_CollStrat::recvReduce
     */
    GTI_ANALYSIS_RETURN recvReduce(DType* data, size_t count, int* groupRanks, size_t groupSize,
                                   int localTargetRank, int localRootRank, size_t remoteCounter,
                                   GroupId groupId, int localOriginRank) {
        return GTI_ANALYSIS_SUCCESS;
    };

    /**
     * @see gti::I_CollStrat::recvAllreduce
     */
    GTI_ANALYSIS_RETURN recvAllreduce(DType* data, size_t count, int* groupRanks, size_t groupSize,
                                      int localTargetRank, int localRootRank, size_t remoteCounter,
                                      GroupId groupId, int localOriginRank) {
        return GTI_ANALYSIS_SUCCESS;
    };

    uint64_t getCollIntraLayerTime() const { return 0; };
}; /*class CollStratMpi*/
} /*namespace gti*/

#endif /* COLL_STRAT_NAIVE_H */
