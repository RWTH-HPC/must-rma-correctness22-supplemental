/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file VectorClockWrapper.h
 *       @see I_VectorClockWrapper.h.
 *
 *  @date 29.06.2021
 *  @author Felix Tomski
 */

#include "I_CommTrack.h"
#include "I_GroupTrack.h"
#include "I_ParallelIdAnalysis.h"
#include "I_VectorClock.h"
#include "I_WinTrack.h"
#include "ModuleBase.h"
#include <sys/time.h>

#include "I_VectorClockWrapper.h"

#ifndef VECTORCLOCKWRAPPER_H
#define VECTORCLOCKWRAPPER_H

using namespace gti;

namespace must {
class VectorClockWrapper : public gti::ModuleBase<VectorClockWrapper, I_VectorClockWrapper> {
  public:
    /**
     * Constructor.
     */
    VectorClockWrapper(const char* instanceName);

    /**
     * Destructor.
     */
    virtual ~VectorClockWrapper(void);

    GTI_ANALYSIS_RETURN init();
    GTI_ANALYSIS_RETURN tick();
    GTI_ANALYSIS_RETURN signal(MustParallelId pId, int isSync, int appRank, int tag,
                               MustCommType comm);
    GTI_ANALYSIS_RETURN wait(MustParallelId pId, int appRank, int tag, MustCommType comm);
    GTI_ANALYSIS_RETURN waitForResponse(MustParallelId pId, int appRank, int tag,
                                        MustCommType comm);

    GTI_ANALYSIS_RETURN allToOne(MustParallelId pId, MustCommType comm, int root);
    GTI_ANALYSIS_RETURN oneToAll(MustParallelId pId, MustCommType comm, int root);
    GTI_ANALYSIS_RETURN allToAll(MustParallelId pId, MustCommType comm);

    GTI_ANALYSIS_RETURN bufferA2aClock(MustParallelId pId, MustCommType comm,
                                       MustRequestType request);
    GTI_ANALYSIS_RETURN bufferA2oClock(MustParallelId pId, int root, MustCommType comm,
                                       MustRequestType request);
    GTI_ANALYSIS_RETURN bufferO2aClock(MustParallelId pId, int root, MustCommType comm,
                                       MustRequestType request);
    GTI_ANALYSIS_RETURN bufferWait(MustParallelId pId, MustCommType comm, MustRequestType request);
    GTI_ANALYSIS_RETURN handleRequest(MustRequestType request, int source, int tag);
    GTI_ANALYSIS_RETURN handleRequestArray(MustRequestType* requests, int* sources, int* tags,
                                           size_t count);
    GTI_ANALYSIS_RETURN handleAnyRequest(MustRequestType* requests, int index, int source, int tag);

    GTI_ANALYSIS_RETURN winAllToAll(MustParallelId pId, MustWinType winHandle);

    GTI_ANALYSIS_RETURN lock(MustParallelId pId, MustWinType winHandle, int appRank, int lock_type);
    GTI_ANALYSIS_RETURN unlock(MustParallelId pId, MustWinType winHandle, int appRank);

    GTI_ANALYSIS_RETURN winStart(MustParallelId pId, MustLocationId lId, MustGroupType groupHandle, MustWinType winHandle);
    GTI_ANALYSIS_RETURN winComplete(MustParallelId pId, MustLocationId lId, MustWinType winHandle);
    GTI_ANALYSIS_RETURN winPost(MustParallelId pId, MustLocationId lId, MustGroupType groupHandle, MustWinType winHandle);
    GTI_ANALYSIS_RETURN winWait(MustParallelId pId, MustLocationId lId, MustWinType winHandle);

    GTI_ANALYSIS_RETURN addPersistentSendInfo(MustParallelId pId, int appRank, int tag,
                                              unsigned long requestHandle, MustCommType comm);
    GTI_ANALYSIS_RETURN addPersistentRecvInfo(MustParallelId pId, MustRequestType request,
                                              MustCommType comm);

  protected:
    I_VectorClock* myVectorClockMod;
    I_CommTrack* myCommTrackMod;
    I_GroupTrack* myGroupTrackMod;
    I_WinTrack* myWinTrackMod;

  private:
    std::set<std::pair<int, MustWinType>> sharedLocks; // track shared locks
    uint64_t myP2pTime;
    uint64_t myCollTime;
    std::map<MustWinType, MustGroupType> myWinCompleteGroupMap;
    std::map<MustWinType, MustGroupType> myWinWaitGroupMap;

    inline uint64_t getUsecTime() const {
       struct timeval t;
       gettimeofday(&t, NULL);
       return t.tv_sec * 1000000 + t.tv_usec;
    }
}; /*class VectorClockWrapper*/
} /*namespace must*/

#endif /*VECTORCLOCKWRAPPER_H*/
