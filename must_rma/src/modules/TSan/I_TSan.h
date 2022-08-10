/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_TSan.h
 *       @see I_TSan.
 *
 *  @date 18.05.2017
 *  @author Simon Schwitanski
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"
#include "I_ChannelId.h"
#include "MustTypes.h"

#ifndef I_TSAN_H
#define I_TSAN_H

typedef unsigned long uptr;

/**
 * Enables communication with a ThreadSanitizer instance.
 */
class I_TSan : public gti::I_Module
{
public:
    /**
     * Wrapper function for AnnotateHappensBefore interface of ThreadSanitizer. Used to
     * define a happens-before arc. The race detector will infer an arc from the
     * begin to the end when they share the same pointer.
     *
     * @param pId parallel id
     * @param lId location id
     * @param cv pointer used to identify happens-before arc
     */
    virtual void annotateHappensBefore(MustParallelId pId, MustLocationId lId, const volatile void *cv) = 0;

    /**
     * Wrapper function for AnnotateHappensAfter interface of ThreadSanitizer. Used to
     * define a destination of a happens-before arc.
     *
     * @param pId parallel id
     * @param lId location id
     * @param cv pointer used to identify happens-before arc
     */
    virtual void annotateHappensAfter(MustParallelId pId, MustLocationId lId, const volatile void *cv) = 0;

    /**
     * Wrapper function for AnnotateInitTLC interface of ThreadSanitizer. Stores
     * the current vector clock as begin of a TLC window.
     *
     * @param pId parallel id
     * @param lId location id
     * @param cv pointer used to identify vector clock
     */
    virtual void annotateInitTLC(MustParallelId pId, MustLocationId lId, const volatile void *cv) = 0;


    /**
     * Wrapper function for AnnotateStartTLC interface of ThreadSanitizer. Loads 
     * the vector clock associated with the given pointer.
     *
     * @param pId parallel id
     * @param lId location id
     * @param cv pointer used to identify vector clock
     */
    virtual void annotateStartTLC(MustParallelId pId, MustLocationId lId, const volatile void *cv) = 0;

    /**
     * Wrapper function for AnnotateMemoryRead interface of ThreadSanitizer.
     * Adds a reading memory access with given address and size.
     *
     * @param pId parallel id
     * @param lId location id
     * @param addr address of memory location
     * @param size size of read memory location
     */
    virtual void annotateMemoryRead(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size) = 0;

    /**
     * Wrapper function for AnnotateMemoryWrite interface of ThreadSanitizer.
     * Adds a reading memory access with given address and size.
     *
     * @param pId parallel id
     * @param lId location id
     * @param addr address of memory location
     * @param size size of written memory location
     */
    virtual void annotateMemoryWrite(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size) = 0;


    /**
     * Wrapper function for AnnotateMemoryRead interface of ThreadSanitizer.
     * Adds a reading memory access with given address and size.
     *
     * @param pId parallel id
     * @param lId location id
     * @param addr address of memory location
     * @param size size of read memory location
     */
    virtual void annotateMemoryReadPC(MustParallelId pId, MustLocationId lId, MustAddressType addr, unsigned long size, void *pc) = 0;

    /**
     * Wrapper function for AnnotateMemoryWrite interface of ThreadSanitizer.
     * Adds a reading memory access with given address and size.
     *
     * @param pId parallel id
     * @param lId location id
     * @param addr address of memory location
     * @param size size of written memory location
     */
    virtual void annotateMemoryWritePC(MustParallelId pId, MustLocationId lId, MustAddressType addr, unsigned long size, void *pc) = 0;

    /**
     * Wrapper function for AnnotateMemoryRead interface of ThreadSanitizer.
     * Adds a reading memory access with given address and size.
     *
     * @param pId parallel id
     * @param lId location id
     * @param addr address of memory location
     * @param size size of read memory location
     */
    virtual void annotateMemoryReadPC(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size, uptr pc) = 0;

    /**
     * Wrapper function for AnnotateMemoryWrite interface of ThreadSanitizer.
     * Adds a reading memory access with given address and size.
     *
     * @param pId parallel id
     * @param lId location id
     * @param addr address of memory location
     * @param size size of written memory location
     */
    virtual void annotateMemoryWritePC(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size, uptr pc) = 0;

    /**
     * Wrapper function for AnnotateRWLockCreate interface of ThreadSanitizer.
     * Creates a reader-writer lock.
     *
     * @param pId parallel id
     * @param lId location id
     * @param cv pointer used to identify lock
     */
    virtual void annotateRWLockCreate(MustParallelId pId, MustLocationId lId, const volatile void *cv) = 0;

    /**
     * Wrapper function for AnnotateRWLockDestroy interface of ThreadSanitizer.
     * Destroys a reader-writer lock.
     *
     * @param pId parallel id
     * @param lId location id
     * @param cv pointer used to identify lock
     */
    virtual void annotateRWLockDestroy(MustParallelId pId, MustLocationId lId, const volatile void *cv) = 0;

    /**
     * Wrapper function for AnnotateRWLockAcquired interface of ThreadSanitizer.
     * Annotates acquirement of a reader-writer lock.
     *
     * @param pId parallel id
     * @param lId location id
     * @param cv pointer used to identify lock
     * @param isWriteLock true iff it is a write lock
     */
    virtual void annotateRWLockAcquired(MustParallelId pId, MustLocationId lId, const volatile void *cv, bool isWriteLock) = 0;

    /**
     * Wrapper function for AnnotateRWLockReleased interface of ThreadSanitizer.
     * Annotates release of a reader-writer lock.
     *
     * @param pId parallel id
     * @param lId location id
     * @param cv pointer used to identify lock
     * @param isWriteUnlock true iff it is a write unlock
     */
    virtual void annotateRWLockReleased(MustParallelId pId, MustLocationId lId, const volatile void *cv, bool isWriteUnlock) = 0;

    /**
     * Wrapper function for AnnotateIgnoreReadsBegin interface of ThreadSanitizer.
     */
    virtual void annotateIgnoreReadsBegin() = 0;

    /**
     * Wrapper function for AnnotateReadsEnd interface of ThreadSanitizer.
     */
    virtual void annotateIgnoreReadsEnd() = 0;

    /**
     * Wrapper function for AnnotateWritesBegin interface of ThreadSanitizer.
     */
    virtual void annotateIgnoreWritesBegin() = 0;

    /**
     * Wrapper function for AnnotateWritesEnd interface of ThreadSanitizer.
     */
    virtual void annotateIgnoreWritesEnd() = 0;

    /**
     * Wrapper function for AnnotateSyncBegin interface of ThreadSanitizer.
     */
    virtual void annotateIgnoreSyncBegin() = 0;

    /**
     * Wrapper function for AnnotateSyncEnd interface of ThreadSanitizer.
     */
    virtual void annotateIgnoreSyncEnd() = 0;

    /**
     * Wrapper function for annotating atomic load (8-bit).
     *
     * @param a address of memory location
     */
    virtual void annotateAtomic8Load(const volatile unsigned char *a) = 0;

    /**
     * Wrapper function for annotating atomic store (8-bit).
     *
     * @param a address of memory location
     */
    virtual void annotateAtomic8Store(volatile unsigned char *a) = 0;

    /**
     * Wrapper function for annotating atomic load (16-bit).
     *
     * @param a address of memory location
     */
    virtual void annotateAtomic16Load(const volatile unsigned short *a) = 0;

    /**
     * Wrapper function for annotating atomic store (16-bit).
     *
     * @param a address of memory location
     */
    virtual void annotateAtomic16Store(volatile unsigned short *a) = 0;

    /**
     * Wrapper function for annotating atomic load (32-bit).
     *
     * @param a address of memory location
     */
    virtual void annotateAtomic32Load(const volatile unsigned int *a) = 0;

    /**
     * Wrapper function for annotating atomic store (32-bit).
     *
     * @param a address of memory location
     */
    virtual void annotateAtomic32Store(volatile unsigned int *a) = 0;

    /**
     * Wrapper function for annotating atomic load (64-bit).
     *
     * @param a address of memory location
     */
    virtual void annotateAtomic64Load(const volatile unsigned long long *a) = 0;

    /**
     * Wrapper function for annotating atomic store (64-bit).
     *
     * @param a address of memory location
     */
    virtual void annotateAtomic64Store(volatile unsigned long long *a) = 0;

    virtual void * createFiber(unsigned flags) = 0;
    virtual void destroyFiber(void* fiber) = 0;
    virtual void switchToFiber(void* fiber, unsigned flags) = 0;
    virtual void * getCurrentFiber() = 0;

    virtual void annotateFuncEntry(MustParallelId pId, MustLocationId lId)=0;
    virtual void annotateFuncEntry(void* pc)=0;
    virtual void annotateFuncEntry(uptr pc)=0;
    virtual void annotateFuncExit()=0;
};/*class I_TSan*/

#endif /*I_TSAN_H*/
