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
 * @file CollStratApi.h
 * 		P call definition for coll strategies.
 *
 * @author Felix Tomski
 * @date 11.09.2021
 */

#include <cstdint>
#include <stddef.h>
#include <stdint.h>

#ifndef COLL_STRAT_API_H
#define COLL_STRAT_API_H

/**
 *
 */
inline int PnaiveBcastSend(const unsigned long long* data, size_t count, const int* groupRanks,
                           size_t groupSize, int localTargetRank, int localRootRank,
                           uint64_t groupId, int localOriginRank) {
    return 0;
}
typedef int (*naiveBcastSendP)(const unsigned long long*, size_t, const int*, size_t, int, int,
                               uint64_t, int, int);

/**
 *
 */
inline int PnaiveReduceSend(const unsigned long long* data, size_t count, const int* groupRanks,
                            size_t groupSize, int localTargetRank, int localRootRank,
                            size_t remoteCounter, uint64_t groupId, int localOriginRank) {
    return 0;
}
typedef int (*naiveReduceSendP)(const unsigned long long*, size_t, const int*, size_t, int, int,
                                size_t, uint64_t, int, int);

/**
 *
 */
inline int PbinomialBcastSend(const unsigned long long* data, size_t count, const int* groupRanks,
                              size_t groupSize, int localTargetRank, int localRootRank,
                              uint64_t groupId, int localOriginRank) {
    return 0;
}
typedef int (*binomialBcastSendP)(const unsigned long long*, size_t, const int*, size_t, int, int,
                                  uint64_t, int, int);

/**
 *
 */
inline int PbinomialReduceSend(const unsigned long long* data, size_t count, const int* groupRanks,
                               size_t groupSize, int localTargetRank, int localRootRank,
                               size_t remoteCounter, uint64_t groupId, int localOriginRank) {
    return 0;
}
typedef int (*binomialReduceSendP)(const unsigned long long*, size_t, const int*, size_t, int, int,
                                   size_t, uint64_t, int, int);

/**
 *
 */
inline int PnaiveAllreduceSend(const unsigned long long* data, size_t count, const int* groupRanks,
                               size_t groupSize, int localTargetRank, int localRootRank,
                               size_t remoteCounter, uint64_t groupId, int localOriginRank) {
    return 0;
}
typedef int (*naiveAllreduceSendP)(const unsigned long long*, size_t, const int*, size_t, int, int,
                                  size_t, uint64_t, int, int);

/**
 *
 */
inline int PbinomialAllreduceSend(const unsigned long long* data, size_t count, const int* groupRanks,
                               size_t groupSize, int localTargetRank, int localRootRank,
                               size_t remoteCounter, uint64_t groupId, int localOriginRank) {
    return 0;
}
typedef int (*binomialAllreduceSendP)(const unsigned long long*, size_t, const int*, size_t, int, int,
                                   size_t, uint64_t, int, int);

#endif /*COLL_STRAT_API_H*/
