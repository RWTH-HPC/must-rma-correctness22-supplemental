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
 * @file I_CollStrat.h
 *       @see I_CollStrat
 *
 * @author Felix Tomski
 */

#include "GtiDefines.h"
#include "GtiEnums.h"
#include "GtiTypes.h"
#include "I_Module.h"

#include <cstring>
#include <stdarg.h>
#include <unordered_map>
#include <vector>

#ifndef I_COLL_STRAT_H
#define I_COLL_STRAT_H

namespace gti {

enum class CollStrat { NAIVE = 0, BINOMIAL, MPI };

inline CollStrat strToSColltrategy(const char* str) {
    if (!str)
        return CollStrat::BINOMIAL;

    if (std::strcmp(str, "binomial") == 0)
        return CollStrat::BINOMIAL;
    if (std::strcmp(str, "mpi") == 0)
        return CollStrat::MPI;
    return CollStrat::NAIVE;
}

using GroupId = uint64_t;
using DType = unsigned long long;

class I_CollStrat : public I_Module {
  public:
    /**
     * Virtual destructor.
     */
    virtual ~I_CollStrat() {}

    /**
     * Performs a broadcast operation of the \p buf from \p root.
     * @param buf For the \p root this is the input buffer, for all other processes of \p groupAppRanks the output buffer.
     * @param count Size of the buffer.
     * @param root The root process for the broadcast opertions.
     * @param myLocalRank The local (within the application group) process ID of the calling app rank.
     * @param groupAppRanks List of application process IDs performing the operation.
     * @param groupId Identifier for the application group.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual GTI_ANALYSIS_RETURN broadcast(DType* buf, size_t count, int root, int myLocalRank,
                                          const std::vector<int>& groupAppRanks,
                                          GroupId groupId) = 0;
    /**
     * Performs a reduce operation of the \p buf from \p root.
     * @param sendbuf Input buffer for all processes of \p groupAppRanks.
     * @param recvbuf Output buffer for the \p root processes. Ignored for all other processes.
     * @param count See broadcast.
     * @param root See broadcast.
     * @param myLocalRank See broadcast.
     * @param groupAppRanks See broadcast.
     * @param groupId See broadcast.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual GTI_ANALYSIS_RETURN reduce(const DType* sendbuf, DType* recvbuf, size_t count, int root,
                                       int myLocalRank, const std::vector<int>& groupAppRanks,
                                       GroupId groupId) = 0;

    /**
     * Performs an allreduce operation on the \p sendbuf's.
     * @param sendbuf Input buffer for all processes of \p groupAppRanks.
     * @param recvbuf Output buffer for all processes of \p groupAppRanks.
     * @param count See broadcast.
     * @param myLocalRank See broadcast.
     * @param groupAppRanks See broadcast.
     * @param groupId See broadcast.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual GTI_ANALYSIS_RETURN allreduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                          int myLocalRank, const std::vector<int>& groupAppRanks,
                                          GroupId groupId) = 0;

    /**
     * Receive a broadcast message from a child in the tree.
     * The received message is directly merged with the own message for the corresponding broadcast.
     */
    virtual GTI_ANALYSIS_RETURN recvBcast(DType* data, size_t count, int* groupRanks,
                                          size_t groupSize, int localTargetRank, int localRootRank,
                                          GroupId groupId, int localOriginRank) = 0;

    /* Only relevant for strategies implementing GtiCollStrat. */
    /**
     * Receive a receive message from a child in the tree.
     * The received message is directly merged with the own message for the corresponding reduction.
     */
    virtual GTI_ANALYSIS_RETURN recvReduce(DType* data, size_t count, int* groupRanks,
                                           size_t groupSize, int localTargetRank, int localRootRank,
                                           size_t remoteCounter, GroupId groupId,
                                           int localOriginRank) = 0;

    /**
     * Dedicated receive function for allreduce.
     */
    virtual GTI_ANALYSIS_RETURN recvAllreduce(DType* data, size_t count, int* groupRanks,
                                              size_t groupSize, int localTargetRank,
                                              int localRootRank, size_t remoteCounter,
                                              GroupId groupId, int localOriginRank) = 0;

    virtual uint64_t getCollIntraLayerTime() const = 0;

}; /*class I_CollStrat*/
} /*namespace gti*/

#endif /* I_COLL_STRAT_H */
