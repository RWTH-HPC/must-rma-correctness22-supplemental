/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TSan.cpp
 *       @see MUST::TSan.
 *
 *  @date 18.05.2017
 *  @author Simon Schwitanski
 */

#include "GtiMacros.h"
#include "TSan.h"
#include "MustEnums.h"

#include <sstream>
#include <string.h>

using namespace must;

mGET_INSTANCE_FUNCTION(TSan)
mFREE_INSTANCE_FUNCTION(TSan)
mPNMPI_REGISTRATIONPOINT_FUNCTION(TSan)

//=============================
// Constructor
//=============================

TSan::TSan (const char* instanceName)
    : gti::ModuleBase<TSan, I_TSan> (instanceName)
{
    //create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUB_MODULES 2
    if (subModInstances.size() < NUM_SUB_MODULES)
    {
    		std::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
    		assert (0);
    }
    if (subModInstances.size() > NUM_SUB_MODULES)
    {
    		for (std::vector<I_Module*>::size_type i = NUM_SUB_MODULES; i < subModInstances.size(); i++)
    			destroySubModuleInstance (subModInstances[i]);
    }

    myLogger = (I_CreateMessage*) subModInstances[0];
    myLIdMod = (I_LocationAnalysis*) subModInstances[1];

    //initFuncNameModule();
    //Initialize module data
    /*Nothing to do*/
}

//=============================
// Destructor
//=============================
TSan::~TSan ()
{
    if (myLogger)
		destroySubModuleInstance ((I_Module*) myLogger);
	myLogger = NULL;

    if (myLIdMod)
        destroySubModuleInstance ((I_LocationAnalysis*) myLIdMod);
    myLIdMod = NULL;
}

void TSan::annotateHappensBefore(MustParallelId pId, MustLocationId lId, const volatile void *cv) {
    TsanHappensBefore(cv);
}

void TSan::annotateHappensAfter(MustParallelId pId, MustLocationId lId, const volatile void *cv) {
    TsanHappensAfter(cv);
}

void TSan::annotateInitTLC(MustParallelId pId, MustLocationId lId, const volatile void *cv) {
    TsanInitTLC(cv);
}

void TSan::annotateStartTLC(MustParallelId pId, MustLocationId lId, const volatile void *cv) {
    TsanStartTLC(cv);
}

void TSan::annotateMemoryRead(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size) {
    TsanMemoryRead((void*) addr, size);
}

void TSan::annotateMemoryReadPC(MustParallelId pId, MustLocationId lId, MustAddressType addr, unsigned long size, void *pc) {
    TsanMemoryReadPC((void*) addr, size, pc);
}

void TSan::annotateMemoryReadPC(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size, uptr pc) {
/*#ifdef USE_CALLPATH
    LocationInfo lInfo = myLIdMod->getInfoForId(pId, lId);
    // TODO: use PC annotation interface of TSan
    TsanMemoryRead((void*) addr, size);
#else*/
    TsanMemoryReadPC((void*) addr, (unsigned long) size, (void*) pc);
//#endif
}

void TSan::annotateMemoryWrite(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size) {
    TsanMemoryWrite((void*) addr, size);
}

void TSan::annotateMemoryWritePC(MustParallelId pId, MustLocationId lId, MustAddressType addr, unsigned long size, void* pc) {
    TsanMemoryWritePC((void*) addr, size, pc);
}



void TSan::annotateMemoryWritePC(MustParallelId pId, MustLocationId lId, MustAddressType addr, size_t size, uptr pc) {
/*#ifdef USE_CALLPATH
    LocationInfo lInfo = myLIdMod->getInfoForId(pId, lId);
    // TODO: use PC annotation interface of TSan
    TsanMemoryWrite((void*) addr, size);
#else*/
    TsanMemoryWritePC((void*) addr, (unsigned long) size, (void*) pc);
//#endif
}

void TSan::annotateRWLockCreate(MustParallelId pId, MustLocationId lId, const volatile void *cv) {
    TsanRWLockCreate(cv);
}

void TSan::annotateRWLockDestroy(MustParallelId pId, MustLocationId lId, const volatile void *cv) {
    TsanRWLockDestroy(cv);
}

void TSan::annotateRWLockAcquired(MustParallelId pId, MustLocationId lId, const volatile void *cv, bool isWriteLock) {
    TsanRWLockAcquired(cv, (unsigned long long) isWriteLock);
}

void TSan::annotateRWLockReleased(MustParallelId pId, MustLocationId lId, const volatile void *cv, bool isWriteUnlock) {
    TsanRWLockReleased(cv, (unsigned long long) isWriteUnlock);
}

void TSan::annotateAtomic8Load(const volatile unsigned char *a) {
    TsanAtomic8Load(a, mo_relaxed);
}

void TSan::annotateAtomic8Store(volatile unsigned char *a) {
    TsanAtomic8Store(a, *a, mo_release);
}

void TSan::annotateAtomic16Load(const volatile unsigned short *a) {
    TsanAtomic16Load(a, mo_relaxed);
}

void TSan::annotateAtomic16Store(volatile unsigned short *a) {
    TsanAtomic16Store(a, *a, mo_release);
}

void TSan::annotateAtomic32Load(const volatile unsigned int *a) {
    TsanAtomic32Load(a, mo_relaxed);
}

void TSan::annotateAtomic32Store(volatile unsigned int *a) {
    TsanAtomic32Store(a, *a, mo_release);
}

void TSan::annotateAtomic64Load(const volatile unsigned long long *a) {
    TsanAtomic64Load(a, mo_relaxed);
}

void TSan::annotateAtomic64Store(volatile unsigned long long *a) {
    TsanAtomic64Store(a, *a, mo_release);
}

void TSan::annotateFuncEntry(MustParallelId pId, MustLocationId lId)
{
  TsanFuncEntry(
    (void*) myLIdMod->getInfoForId(pId, lId).codeptr
  );
}

void TSan::annotateFuncEntry(void* pc)
{
  TsanFuncEntry(
    pc
  );
}

void TSan::annotateFuncEntry(uptr pc)
{
  TsanFuncEntry(
    (void*) pc
  );
}

void TSan::annotateFuncExit()
{
  TsanFuncExit();
}

void TSan::annotateIgnoreReadsBegin()
{
  TsanIgnoreReadsBegin();
}

void TSan::annotateIgnoreReadsEnd()
{
  TsanIgnoreReadsEnd();
}

void TSan::annotateIgnoreWritesBegin()
{
  TsanIgnoreWritesBegin();
}

void TSan::annotateIgnoreWritesEnd()
{
  TsanIgnoreWritesEnd();
}

void TSan::annotateIgnoreSyncBegin()
{
  TsanIgnoreSyncBegin();
}

void TSan::annotateIgnoreSyncEnd()
{
  TsanIgnoreSyncEnd();
}

void * TSan::createFiber(unsigned flags)
{
  return TsanCreateFiber(flags);
}

void TSan::destroyFiber(void* fiber)
{
  TsanDestroyFiber(fiber);
}

void TSan::switchToFiber(void* fiber, unsigned flags)
{
  TsanSwitchToFiber(fiber, flags);
}

void * TSan::getCurrentFiber()
{
  return TsanGetCurrentFiber();
}

void TSan::setFiberName(void* fiber, const char* name)
{
  return TsanSetFiberName(fiber, name);
}

/*EOF*/
