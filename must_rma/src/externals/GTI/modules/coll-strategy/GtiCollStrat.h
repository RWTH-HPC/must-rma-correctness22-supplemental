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
 *       Common class for strategies using the GTI communication infrastructure.
 *
 * @author Felix Tomski
 * @date 11.09.2021
 */

#include "CollStratApi.h"
#include "I_CollStrat.h"
#include "ModuleBase.h"

#include "Bitset.h"
#include <deque>
#include <queue>
#include <unordered_map>
#include <vector>
#include <sys/time.h>

#ifndef GTI_COLL_STRAT_H
#define GTI_COLL_STRAT_H

namespace gti {

template <typename T>
GTI_ANALYSIS_RETURN reduce_pairwise(const T* src, T* dest, std::size_t count) {
    for (std::size_t i = 0; i < count; i++) {
        if (src[i] > dest[i])
            dest[i] = src[i];
    }

    return GTI_ANALYSIS_SUCCESS;
}

class GtiCollStrat {
  protected:
    uint64_t myIntraLayerTime;
    uint64_t getUsecTime() const {
       struct timeval t;
       gettimeofday(&t, NULL);
       return t.tv_sec * 1000000 + t.tv_usec;
    }

    enum class ONGOING_COLL_TYPE { NONE = 0, BCAST, REDUCE, ALLREDUCE };
    class InplaceData_t {
      public:
        DType* ptr;
        std::size_t numWaitingReduce;
        int root;
        GroupId gId;
        ONGOING_COLL_TYPE type;
        InplaceData_t() : ptr(nullptr), type(ONGOING_COLL_TYPE::NONE){};
        void set(DType* p, int r, GroupId id, ONGOING_COLL_TYPE t, std::size_t nWaitReduce = 0) {
            ptr = p;
            root = r;
            gId = id;
            type = t;
            numWaitingReduce = nWaitReduce;
        }
    };

    using buf_t = std::vector<DType>;
    template <typename T> using gmap_t = std::unordered_map<GroupId, T>;

    /* Buffers for messages that arrive before the process has entered the analysis/application call. */
    gmap_t<std::queue<buf_t>> myBcastBuffer;
    gmap_t<std::deque<std::pair<size_t, buf_t>>> myReduceBuffer; /* First pair entry is the number of clocks to wait for. */
    const size_t INVALID_ALLREDUCE_BUF = -1;
    gmap_t<std::pair<size_t, buf_t>> myAllreduceBuffer;
    /* For reduce and allreduce operations we may even receive messages for different ongoing operations. */
    gmap_t<size_t> myReduceCounters;
    gmap_t<size_t> myAllreduceCounters;

    /* Buffer groups and their complement */
    gmap_t<std::vector<int>> myGroups;
    gmap_t<std::vector<int>> myCompGroups;

    InplaceData_t myInplaceData; /* Information for an ongoing in-place operation */

    I_Place* myPlaceMod;
    std::size_t myNumProcs;

    GtiCollStrat() : myPlaceMod(nullptr), myNumProcs(0), myInplaceData() {};

    /* More or less an own version of try_emplace (C++17) for unordered_map's */
    template <typename KeyT, typename ContainerT, typename... Args>
    auto findOrAdd(ContainerT& c, KeyT&& id, Args&&... args)
        -> std::pair<decltype(c.find(id)), bool> {
        bool wasAdded = false;
        auto it = c.find(id);
        if (it == c.end()) {
            it = c.emplace_hint(it, std::piecewise_construct,
                                std::forward_as_tuple(std::forward<KeyT>(id)),
                                std::forward_as_tuple(std::forward<Args>(args)...));
            wasAdded = true;
        }

        return std::make_pair(it, wasAdded);
    }

    auto findOrAddReduceCounter(GroupId id)
        -> std::pair<decltype(myReduceCounters.find(id)), bool> {
        return findOrAdd(myReduceCounters, id, 0);
    }

    auto findOrAddAllreduceCounter(GroupId id)
        -> std::pair<decltype(myAllreduceCounters.find(id)), bool> {
        return findOrAdd(myAllreduceCounters, id, 0);
    }

    template <typename... Args>
    auto findOrAddGroup(GroupId id, Args&&... args)
        -> std::pair<decltype(myGroups.find(id)), bool> {
        return findOrAdd(myGroups, id, std::forward<Args>(args)...);
    }

    const std::vector<int>& findOrAddCompGroup(GroupId id, const std::vector<int>& groupRanks) {
        auto it = myCompGroups.find(id);
        if (it == myCompGroups.end()) {
            it = myCompGroups.emplace_hint(it, std::piecewise_construct, std::forward_as_tuple(id),
                                           std::forward_as_tuple(myNumProcs - groupRanks.size()));
            const auto& world = myGroups[0];
            std::set_difference(world.begin(), world.end(), groupRanks.begin(), groupRanks.end(),
                                it->second.begin());
        }

        return it->second;
    }

}; /*class GtiCollStrat*/
} /*namespace gti*/

#endif /* GTI_COLL_STRAT_H */
