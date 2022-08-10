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
 * @file VectorClockApi.h
 * 		P call definition for implicitly added GTI API.
 *
 * @author Felix Tomski
 * @date 30.05.2021
 */

#include <stdint.h>
#include <stddef.h>

#ifndef VECTORCLOCK_API_H
#define VECTORCLOCK_API_H

inline int PpassVClockAcrossP2P(unsigned long long* vectorClock,
                                size_t size,
                                int originId,
                                int isSync,
                                int isResponse,
                                uint64_t queueId) {return 0;}

typedef int (*passVClockAcrossP2PP) (unsigned long long* vectorClock,
                                    size_t size,
                                    int originId,
                                    int isSync,
                                    int isResponse,
                                    uint64_t queueId,
                                    int placeId);

inline int PpassUnlockClockToProxy(unsigned long long* vectorClock,
                                size_t clockSize,
                                uint64_t lockHandle,
                                int originId) {return 0;}

typedef int (*passUnlockClockToProxyP) (unsigned long long* vectorClock,
                                    size_t clockSize,
                                    uint64_t lockHandle,
                                    int originId,
                                    int placeId);

inline int PpassLockNotify(uint64_t lockHandle,
                                int originId) {return 0;}

typedef int (*passLockNotifyP) (uint64_t lockHandle,
                                    int originId,
                                    int placeId);

inline int PpassUnlockClockToEnd(unsigned long long* vectorClock,
                                size_t clockSize,
                                uint64_t lockHandle,
                                int originId) {return 0;}

typedef int (*passUnlockClockToEndP) (unsigned long long* vectorClock,
                                    size_t clockSize,
                                    uint64_t lockHandle,
                                    int originId,
                                    int placeId);

inline void PsyncNotify (int remoteRank,
                         int ownRank) {}

typedef void (*syncNotifyP) (int remoteRank,
                             int ownRank);

#endif /*VECTORCLOCK_API_H*/
