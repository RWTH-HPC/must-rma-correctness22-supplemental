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
 * @file CollStratMpi.cpp
 *
 * @author Felix Tomski
 * @date 11.09.2021
 */

#include "CollStratMpi.h"
#include "GtiCollStrat.h"
#include "GtiEnums.h"
#include "GtiMacros.h"

#include <assert.h>
#include <mpi.h>

using namespace gti;

//=============================
// Basic Module functions
//=============================
mGET_INSTANCE_FUNCTION(CollStratMpi) mFREE_INSTANCE_FUNCTION(CollStratMpi)
    mPNMPI_REGISTRATIONPOINT_FUNCTION(CollStratMpi)

    //=============================
    // CollStratMpi
    //=============================
    CollStratMpi::CollStratMpi(const char* instanceName)
    : ModuleBase<CollStratMpi, I_CollStrat>(instanceName), myBcastBuffer() {
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances();

    // Needs no sub modules
    assert(subModInstances.empty());

    PNMPI_modHandle_t handle;
    PNMPI_Service_descriptor_t service;
    PNMPI_Service_Fct_t fct;

    int err = PNMPI_Service_GetModuleByName("split_processes", &handle);
    assert(err == PNMPI_SUCCESS);
    err = PNMPI_Service_GetServiceByName(handle, "SplitMod_getMySetComm", "p", &service);
    assert(err == PNMPI_SUCCESS);
    MPI_Comm fakeComm;
    ((int (*)(void*))service.fct)(&fakeComm);
    XMPI_Comm_dup(fakeComm, &myToolComm);
    XMPI_Comm_group(myToolComm, &myToolGroup);
    myCommunicators.insert({0, myToolComm});

    int myGroupRank, myWorldRank;
    XMPI_Comm_rank(MPI_COMM_WORLD, &myWorldRank);
    XMPI_Group_rank(myToolGroup, &myGroupRank);

    int numToolProcs;
    XMPI_Comm_size(myToolComm, &numToolProcs);
    myBcastBuffer.reserve(numToolProcs);
}

//=============================
// ~CollStratMpi
//=============================
CollStratMpi::~CollStratMpi(void) {}

auto CollStratMpi::findOrAddComm(GroupId id, const std::vector<int>& ranks)
    -> decltype(myCommunicators.find(id)->second) {
    auto it = myCommunicators.find(id);
    if (it == myCommunicators.end()) {
        MPI_Group new_group;
        XMPI_Group_incl(myToolGroup, ranks.size(), ranks.data(), &new_group);
        MPI_Comm new_comm;
        XMPI_Comm_create_group(myToolComm, new_group, id, &new_comm);
        it = myCommunicators.insert(it, {id, new_comm});
    }

    return it->second;
}

//=============================
// broadcast
//=============================
GTI_ANALYSIS_RETURN CollStratMpi::broadcast(DType* buf, size_t count, int root, int myLocalRank,
                                            const std::vector<int>& groupAppRanks,
                                            GroupId groupId) {
    GTI_ANALYSIS_RETURN res;
    DType* tmpPtr = buf;
    if (myLocalRank != root) {
        myBcastBuffer.resize(count);
        tmpPtr = myBcastBuffer.data();
    }

    res = XMPI_Bcast(tmpPtr, count, MPI_UNSIGNED_LONG_LONG, root,
                     findOrAddComm(groupId, groupAppRanks)) == PNMPI_SUCCESS
              ? GTI_ANALYSIS_SUCCESS
              : GTI_ANALYSIS_FAILURE;

    if (myLocalRank != root) {
        reduce_pairwise(tmpPtr, buf, count);
    }

    return res;
}

//=============================
// reduce
//=============================
GTI_ANALYSIS_RETURN CollStratMpi::reduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                         int root, int myLocalRank,
                                         const std::vector<int>& groupAppRanks, GroupId groupId) {
    return XMPI_Reduce(myLocalRank == root ? MPI_IN_PLACE : sendbuf, recvbuf, count,
                       MPI_UNSIGNED_LONG_LONG, MPI_MAX, root,
                       findOrAddComm(groupId, groupAppRanks)) == PNMPI_SUCCESS
               ? GTI_ANALYSIS_SUCCESS
               : GTI_ANALYSIS_FAILURE;
}

//=============================
// allreduce
//=============================
GTI_ANALYSIS_RETURN CollStratMpi::allreduce(const DType* sendbuf, DType* recvbuf, size_t count,
                                            int myLocalRank, const std::vector<int>& groupAppRanks,
                                            GroupId groupId) {
    return XMPI_Allreduce(MPI_IN_PLACE, recvbuf, count, MPI_UNSIGNED_LONG_LONG, MPI_MAX,
                          findOrAddComm(groupId, groupAppRanks)) == PNMPI_SUCCESS
               ? GTI_ANALYSIS_SUCCESS
               : GTI_ANALYSIS_FAILURE;
}
