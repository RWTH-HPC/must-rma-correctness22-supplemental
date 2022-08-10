/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TSan.h
 *       @see MUST::TSan.
 *
 *  @date 18.05.2017
 *  @author Simon Schwitanski
 */

#include "ModuleBase.h"
#include "CompletionTree.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"
#include "I_CreateMessage.h"

#include "I_TSan.h"
#include "TSan_External.h"

#include <string>
#include <stdio.h>

#ifndef TSAN_H
#define TSAN_H

using namespace gti;

namespace must
{
    /**
     * Implementation of I_TSan.
     */
    class TSan : public gti::ModuleBase<TSan, I_TSan>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
        TSan (const char* instanceName);

        ~TSan();

        void annotateHappensBefore(MustParallelId pId, MustLocationId lId, const volatile void *cv);
        void annotateHappensAfter(MustParallelId pId, MustLocationId lId, const volatile void *cv);
        void annotateInitTLC(MustParallelId pId, MustLocationId lId, const volatile void *cv);
        void annotateStartTLC(MustParallelId pId, MustLocationId lId, const volatile void *cv);
        void annotateMemoryRead(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size);
        void annotateMemoryWrite(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size);
        void annotateMemoryReadPC(MustParallelId pId, MustLocationId lId, MustAddressType addr, unsigned long size, void* pc);
        void annotateMemoryWritePC(MustParallelId pId, MustLocationId lId, MustAddressType addr, unsigned long size, void* pc);
        void annotateMemoryReadPC(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size, uptr pc);
        void annotateMemoryWritePC(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size, uptr pc);
        void annotateRWLockCreate(MustParallelId pId, MustLocationId lId, const volatile void *cv);
        void annotateRWLockDestroy(MustParallelId pId, MustLocationId lId, const volatile void *cv);
        void annotateRWLockAcquired(MustParallelId pId, MustLocationId lId, const volatile void *cv, bool isWriteLock);
        void annotateRWLockReleased(MustParallelId pId, MustLocationId lId, const volatile void *cv, bool isWriteUnlock);
        void annotateAtomic8Load(const volatile unsigned char *a);
        void annotateAtomic8Store(volatile unsigned char *a);
        void annotateAtomic16Load(const volatile unsigned short *a);
        void annotateAtomic16Store(volatile unsigned short *a);
        void annotateAtomic32Load(const volatile unsigned int *a);
        void annotateAtomic32Store(volatile unsigned int *a);
        void annotateAtomic64Load(const volatile unsigned long long *a);
        void annotateAtomic64Store(volatile unsigned long long *a);

        void annotateIgnoreReadsBegin();
        void annotateIgnoreReadsEnd();
        void annotateIgnoreWritesBegin();
        void annotateIgnoreWritesEnd();
        void annotateIgnoreSyncBegin();
        void annotateIgnoreSyncEnd();

	    void annotateFuncEntry(MustParallelId pId, MustLocationId lId);
	    void annotateFuncEntry(void* pc);
        void annotateFuncEntry(uptr pc);
	    void annotateFuncExit();

        void * createFiber(unsigned flags);
        void destroyFiber(void* fiber);
        void switchToFiber(void* fiber, unsigned flags);
        void * getCurrentFiber();
        void setFiberName(void* fiber, const char* name);

    protected:
        I_CreateMessage* myLogger;
        I_LocationAnalysis* myLIdMod;
    };
} /*namespace MUST*/

#endif /*TSAN_H*/
