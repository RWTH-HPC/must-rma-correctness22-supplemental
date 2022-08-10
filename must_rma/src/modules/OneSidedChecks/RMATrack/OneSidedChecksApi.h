/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OneSidedChecksApi.h
 * 		P call definition for MUST OneSidedChecks API calls.
 *
 * @author Simon Schwitanski
 * @date 13.06.2017
 */

#ifndef ONESIDEDCHECKS_API_H
#define ONESIDEDCHECKS_API_H

#include "BaseIds.h"

inline int PpassTargetRMAOpAcross (
        int origin,
        MustRMAId rmaId,
        MustParallelId pId,
        MustLocationId lId,
        bool isStore,
        bool isAtomic,
        bool isLocked,
        int target,
        int disp,
        int count,
        MustRemoteIdType originDatatype,
        MustRemoteIdType targetDatatype,
        MustRemoteIdType win,
        int epoch,
        int accessMode,
        unsigned long long clockValue,
        void* returnAddr,
        void* functionAddr) {return 0;}

typedef int (*passTargetRMAOpAcrossP) (
        int origin,
        MustRMAId rmaId,
        MustParallelId pId,
        MustLocationId lId,
        bool isStore,
        bool isAtomic,
        bool isLocked,
        int target,
        int disp,
        int count,
        MustRemoteIdType originDatatype,
        MustRemoteIdType targetDatatype,
        MustRemoteIdType win,
        int epoch,
        int accessMode,
        unsigned long long clockValue,
        void* returnAddr,
        void* functionAddr,
        int toPlaceId);

inline int PpassTargetCompletionAcross (
        MustParallelId pId,
        MustLocationId lId,
        int origin,
        int target,
        MustRemoteIdType win,
        int epoch) {return 0;}

typedef int (*passTargetCompletionAcrossP) (
        MustParallelId pId,
        MustLocationId lId,
        int origin,
        int target,
        MustRemoteIdType win,
        int epoch,
        int toPlaceId);

inline void PnotifySync (
        int remoteRank,
        int ownRank) {}

typedef void (*notifySyncP) (
        int remoteRank,
        int ownRank);

inline void PnotifyOriginOpStart (
        MustRMAId rmaId) {}

typedef void (*notifyOriginOpStartP) (
        MustRMAId rmaId);

inline void PnotifyOriginOpComplete (
        MustParallelId pId,
        MustLocationId lId,
        MustRMAId* rmaId,
        int rmaIdLen) {}

typedef void (*notifyOriginOpCompleteP) (
        MustParallelId pId,
        MustLocationId lId,
        MustRMAId* rmaId,
        int rmaIdLen);

inline void PnotifyTargetOpStart (
        MustRMAId rmaId) {}

typedef void (*notifyTargetOpStartP) (
        MustRMAId rmaId);

inline void PnotifyTargetOpComplete (
        MustParallelId pId,
        MustLocationId lId,
        MustRMAId* rmaId,
        int rmaIdLen) {}

typedef void (*notifyTargetOpCompleteP) (
        MustParallelId pId,
        MustLocationId lId,
        MustRMAId* rmaId,
        int rmaIdLen);


inline void PpropagateGetCall (
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType originaddr,
        int origincount,
        MustDatatypeType origintype,
        int target,
        int targetdisp,
        int targetcount,
        MustDatatypeType targettype,
        MustWinType win,
        MustRequestType request,
        void* ann,
        void* functionaddr) {}

typedef void (*propagateGetCallP) (
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType originaddr,
        int origincount,
        MustDatatypeType origintype,
        int target,
        int targetdisp,
        int targetcount,
        MustDatatypeType targettype,
        MustWinType win,
        MustRequestType request,
        void* ann,
        void* functionaddr);

inline void PpropagatePutCall (
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType originaddr,
        int origincount,
        MustDatatypeType origintype,
        int target,
        int targetdisp,
        int targetcount,
        MustDatatypeType targettype,
        MustWinType win,
        MustRequestType request,
        void* ann,
        void* functionaddr) {}

typedef void (*propagatePutCallP) (
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType originaddr,
        int origincount,
        MustDatatypeType origintype,
        int target,
        int targetdisp,
        int targetcount,
        MustDatatypeType targettype,
        MustWinType win,
        MustRequestType request,
        void* ann,
        void* functionaddr);

inline void PpropagateAccumulateCall (
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType originaddr,
        int origincount,
        MustDatatypeType origintype,
        int target,
        int targetdisp,
        int targetcount,
        MustDatatypeType targettype,
        MustOpType optype,
        MustWinType win,
        MustRequestType request,
        void* ann,
        void* functionaddr) {}

typedef void (*propagateAccumulateCallP) (
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType originaddr,
        int origincount,
        MustDatatypeType origintype,
        int target,
        int targetdisp,
        int targetcount,
        MustDatatypeType targettype,
        MustOpType optype,
        MustWinType win,
        MustRequestType request,
        void* ann,
        void* functionaddr);

inline void PpropagateGetAccumulateCall (
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType originaddr,
        int origincount,
        MustDatatypeType origintype,
        MustAddressType resultaddr,
        int resultcount,
        MustDatatypeType resulttype,
        int target,
        int targetdisp,
        int targetcount,
        MustDatatypeType targettype,
        MustOpType optype,
        MustWinType win,
        MustRequestType request,
        void* ann,
        void* functionaddr) {}

typedef void (*propagateGetAccumulateCallP) (
        MustParallelId pId,
        MustLocationId lId,
        MustAddressType originaddr,
        int origincount,
        MustDatatypeType origintype,
        MustAddressType resultaddr,
        int resultcount,
        MustDatatypeType resulttype,
        int target,
        int targetdisp,
        int targetcount,
        MustDatatypeType targettype,
        MustOpType optype,
        MustWinType win,
        MustRequestType request,
        void* ann,
        void* functionaddr);

inline void PpropagateWinCreate (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinCreateP) (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann);

inline void PpropagateWinLock (
        MustParallelId pId,
        MustLocationId lId,
        int lock_type,
        int rank,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinLockP) (
        MustParallelId pId,
        MustLocationId lId,
        int lock_type,
        int rank,
        MustWinType win,
        void* ann);

inline void PpropagateWinSync (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinSyncP) (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann);

inline void PpropagateWinUnlock (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinUnlockP) (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win,
        void* ann);

inline void PpropagateWinUnlockAll (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinUnlockAllP) (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann);

inline void PpropagateWinFence (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinFenceP) (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann);

inline void PpropagateWinStart (
        MustParallelId pId,
        MustLocationId lId,
        MustGroupType group,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinStartP) (
        MustParallelId pId,
        MustLocationId lId,
        MustGroupType group,
        MustWinType win,
        void* ann);

inline void PpropagateWinComplete (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinCompleteP) (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann);

inline void PpropagateWinPost (
        MustParallelId pId,
        MustLocationId lId,
        MustGroupType group,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinPostP) (
        MustParallelId pId,
        MustLocationId lId,
        MustGroupType group,
        MustWinType win,
        void* ann);

inline void PpropagateWinWait (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinWaitP) (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann);

inline void PpropagateWinFlush(
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinFlushP) (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win,
        void* ann);

inline void PpropagateWinFlushLocal (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinFlushLocalP) (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustWinType win,
        void* ann);

inline void PpropagateWinFlushAll (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinFlushAllP) (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann);

inline void PpropagateWinFlushLocalAll (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann) {}

typedef void (*propagateWinFlushLocalAllP) (
        MustParallelId pId,
        MustLocationId lId,
        MustWinType win,
        void* ann);

inline void PpropagateSend (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustCommType comm,
        void* ann) {}

typedef void (*propagateSendP) (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustCommType comm,
        void* ann);

inline void PpropagateRecv (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustCommType comm,
        void* ann) {}

typedef void (*propagateRecvP) (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustCommType comm,
        void* ann);

inline void PpropagateSendRecv (
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        int source,
        MustCommType comm,
        void* ann) {}

typedef void (*propagateSendRecvP) (
        MustParallelId pId,
        MustLocationId lId,
        int dest,
        int source,
        MustCommType comm,
        void* ann);

inline void PpropagateIsend (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustCommType comm,
        MustRequestType request,
        void* ann) {}

typedef void (*propagateIsendP) (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustCommType comm,
        MustRequestType request,
        void* ann);

inline void PpropagateIrecv (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustCommType comm,
        MustRequestType request) {}

typedef void (*propagateIrecvP) (
        MustParallelId pId,
        MustLocationId lId,
        int rank,
        MustCommType comm,
        MustRequestType request);

inline void PpropagateColl (
        MustParallelId pId,
        MustLocationId lId,
        MustCommType comm,
        int hasRequest,
        MustRequestType request,
        void* ann) {}

typedef void (*propagateCollP) (
        MustParallelId pId,
        MustLocationId lId,
        MustCommType comm,
        int hasRequest,
        MustRequestType request,
        void* ann);

inline void PpropagateCompletedIsend (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType request) {}

typedef void (*propagateCompletedIsendP) (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType request);

inline void PpropagateCompletedIrecv (
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustRequestType request,
        void* ann) {}

typedef void (*propagateCompletedIrecvP) (
        MustParallelId pId,
        MustLocationId lId,
        int source,
        MustRequestType request,
        void* ann);

inline void PpropagateCompletedIcoll (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType request,
        void* ann) {}

typedef void (*propagateCompletedIcollP) (
        MustParallelId pId,
        MustLocationId lId,
        MustRequestType request,
        void* ann);

inline void PhappensBeforeSync (
        void* ann) {}

typedef void (*happensBeforeSyncP) (
        void *ann);

inline void PhappensAfterSync (
        void* ann) {}

typedef void (*happensAfterSyncP) (
        void *ann);

inline void PpropagateSignal (
        void* ann) {}

typedef void (*propagateSignalP) (
        void *ann);

#endif
