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
 * @file CollStratTemplate.h
 *       An implementation of the collective broadcast strategy interface.
 *
 * @author Felix Tomski
 */

#include "I_CollStrat.h"
#include "I_Module.h"
#include "ModuleBase.h"

#ifndef COLL_STRAT_TEMPLATE_H
#define COLL_STRAT_TEMPLATE_H

namespace gti {
class CollStratTemplate : public ModuleBase<CollStratTemplate, I_CollStrat> {
  public:
    /**
     * Constructor.
     * @ref ModConf - The module configuration syntax
     * @param intanceName name of the module instance.
     */
    CollStratTemplate(const char* instanceName);

    /**
     * Destructor.
     */
    ~CollStratTemplate(void);

    /**
     * @see gti::I_CollStrat::broadcast
     */
    GTI_ANALYSIS_RETURN broadcast(DType* buf, size_t count, int root, int myLocalRank,
                                  const std::vector<int>& groupAppRanks, GroupId groupId);

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

    GTI_ANALYSIS_RETURN recvBcast(DType* data, size_t count, int* groupRanks, size_t groupSize,
                                  int localTargetRank, int localRootRank, GroupId groupId,
                                  int localOriginRank);

    GTI_ANALYSIS_RETURN recvReduce(DType* data, size_t count, int* groupRanks, size_t groupSize,
                                   int localTargetRank, int localRootRank, size_t remoteCounter,
                                   GroupId groupId, int localOriginRank);

    GTI_ANALYSIS_RETURN recvAllreduce(DType* data, size_t count, int* groupRanks, size_t groupSize,
                                      int localTargetRank, int localRootRank, size_t remoteCounter,
                                      GroupId groupId, int localOriginRank);
    uint64_t getCollIntraLayerTime() const { return 0; };
}; /*class CollStratTemplate*/
} /*namespace gti*/

#endif /* COLL_STRAT_TEMPLATE_H */
