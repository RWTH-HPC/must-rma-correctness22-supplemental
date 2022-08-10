/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file OnlyOnRootConditionApi.h
 *      Functions used as API calls for the "only on root" preconditioner @see I_OnlyOnRootCondition.
 *
 * @author Mathias Korepkat, Tobias Hilbrich, Joachim Protze
 * @date 23.08.2011
 */

#include "MustEnums.h"
#include "MustTypes.h"
#include "I_DatatypeTrack.h"
#include "I_CommTrack.h"
#include <iostream>

#ifndef ONLYONROOTCONDITIONAPI_H
#define ONLYONROOTCONDITIONAPI_H

//==Gatherv/Scatterv recv/send with counts and displs
inline int PMustOnRootTransferCounts (
        MustParallelId pId,
        MustLocationId lId,
        int isSend, /*True if send transfer, false otherwise*/
        MustAddressType buf,
        MustArgumentId bufArgId,
        const int* counts,
        MustArgumentId countsArgId,
        const int* displs,
        MustArgumentId displsArgId,
        MustDatatypeType type,
        MustArgumentId typeArgId,
        int commSize
)  {return 0;}

typedef int (*MustOnRootTransferCountsP) (
        MustParallelId pId,
        MustLocationId lId,
        int isSend, /*True if send transfer, false otherwise*/
        MustAddressType buf,
        MustArgumentId bufArgId,
        const int* counts,
        MustArgumentId countsArgId,
        const int* displs,
        MustArgumentId displsArgId,
        MustDatatypeType type,
        MustArgumentId typeArgId,
        int commSize
);

//==Gather/Scatter recv/send.
inline int PMustOnRootTransfer (
        MustParallelId pId,
        MustLocationId lId,
        int isSend, /*True if send transfer, false otherwise*/
        MustAddressType buf,
        MustArgumentId bufArgId,
        int count,
        MustArgumentId countArgId,
        MustDatatypeType type,
        MustArgumentId typeArgId
)  {return 0;}

typedef int (*MustOnRootTransferP) (
        MustParallelId pId,
        MustLocationId lId,
        int isSend, /*True if send transfer, false otherwise*/
        MustAddressType buf,
        MustArgumentId bufArgId,
        int count,
        MustArgumentId countArgId,
        MustDatatypeType type,
        MustArgumentId typeArgId
);

#endif /* ONLYONROOTCONDITIONAPI_H */
