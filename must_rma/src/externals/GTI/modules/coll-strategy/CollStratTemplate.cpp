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
 * @file CommProtocolTemplate.cpp
 *       Template for implementations of the collective
 *       broadcast interface.
 *
 * All functions of the interface are offered, but
 * will lead to an assertion.
 *
 */

#include <assert.h>
#include <mpi.h>
#include <pnmpimod.h>
#include <stdio.h>
#include <stdlib.h>

#include "CollStratTemplate.h"
#include "GtiMacros.h"

using namespace gti;

mGET_INSTANCE_FUNCTION(CollStratTemplate) mFREE_INSTANCE_FUNCTION(CollStratTemplate)
    mPNMPI_REGISTRATIONPOINT_FUNCTION(CollStratTemplate)

    //=============================
    // CommProtocolTemplate
    //=============================
    CollStratTemplate::CollStratTemplate(const char* instanceName)
    : ModuleBase<CollStratTemplate, I_CollStrat>(instanceName) {
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances();

    // A Comm Protocol likely needs no sub modules
    assert(subModInstances.empty());
}

//=============================
// ~CollStratTemplate
//=============================
CollStratTemplate::~CollStratTemplate(void) {}

//=============================
// broadcast
//=============================
GTI_ANALYSIS_RETURN CollStratTemplate::broadcast(DType* buf, size_t count, int root,
                                                 int myLocalRank,
                                                 const std::vector<int>& groupAppRanks,
                                                 GroupId groupId) {
    assert(0);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// reduce
//=============================
GTI_ANALYSIS_RETURN CollStratTemplate::reduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                              int root, int myLocalRank,
                                              const std::vector<int>& groupAppRanks,
                                              GroupId groupId) {
    assert(0);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// allreduce
//=============================
GTI_ANALYSIS_RETURN CollStratTemplate::allreduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                                 int myLocalRank,
                                                 const std::vector<int>& groupAppRanks,
                                                 GroupId groupId) {
    assert(0);
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvBcast
//=============================
GTI_ANALYSIS_RETURN CollStratTemplate::recvBcast(DType* data, size_t count, int* groupRanks,
                                                 size_t groupSize, int localTargetRank,
                                                 int localRootRank, GroupId groupId,
                                                 int localOriginRank) {
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvReduce
//=============================
GTI_ANALYSIS_RETURN CollStratTemplate::recvReduce(DType* data, size_t count, int* groupRanks,
                                                  size_t groupSize, int localTargetRank,
                                                  int localRootRank, size_t remoteCounter,
                                                  GroupId groupId, int localOriginRank) {
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// recvAllreduce
//=============================
GTI_ANALYSIS_RETURN CollStratTemplate::recvAllreduce(DType* data, size_t count, int* groupRanks,
                                                     size_t groupSiz, int localTargetRank,
                                                     int localRootRanke, size_t remoteCounter,
                                                     GroupId groupId, int localOriginRank) {
    return GTI_ANALYSIS_SUCCESS;
}
