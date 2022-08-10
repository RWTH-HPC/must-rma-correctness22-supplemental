/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file BaseApi.h
 * 		P call definition for MustBase API calls.
 *
 * @author Tobias Hilbrich
 * @date 10.01.2011
 */

#ifndef BASE_API_H
#define BASE_API_H

#include "BaseIds.h"
#include "mustConfig.h"

// handleNewLocation
inline int PhandleNewLocation (
		MustParallelId pId,
		MustLocationId lId,
		const char* callName,
		int callNameLen,
		const void* codeptr,
		const char* fname,
		size_t fnameLen,
		const void* fbase
#ifdef ENABLE_STACKTRACE
    ,
    int numStackLevels, int stackInfosLength, /*Total length of char array for
                                                 all entries of stack levels*/
    int indicesLength,     /*=numStackLevels*3; used for convenience*/
    int *infoIndices,      /*Array of size 3*numStackLevels with indices to the
                              individual information pieces, for each level
                              infoIndices[level*3+0]=symName,
                              infoIndices[level*3+1]=lineOffset,
                              infoIndices[level*3+2]=fileModule*/
    const char *stackInfos /*All information pieces of all stack levels
                              concatenated, of length stackInfosLength*/
#endif
) {return 0;}

typedef int (*handleNewLocationP) (
		MustParallelId pId,
		MustLocationId lId,
		const char* callName,
		int callNameLen,
		const void* codeptr,
		const char* fname,
		size_t fnameLen,
		const void* fbase
#ifdef ENABLE_STACKTRACE
    ,
    int numStackLevels, int stackInfosLength, /*Total length of char array for
                                                 all entries of stack levels*/
    int indicesLength,     /*=numStackLevels*3; used for convenience*/
    int *infoIndices,      /*Array of size 3*numStackLevels with indices to the
                              individual information pieces, for each level
                              infoIndices[level*3+0]=symName,
                              infoIndices[level*3+1]=lineOffset,
                              infoIndices[level*3+2]=fileModule*/
    const char *stackInfos /*All information pieces of all stack levels
                              concatenated, of length stackInfosLength*/
#endif
);

// Call to pass a location ID to another place on the same level
inline int PpassLocationAcross (
        MustParallelId pId,
        MustLocationId lId,
        const char* callName,
        int callNameLen,
        const void* codeptr,
        const char* fname,
        size_t fnameLen,
        const void* fbase
#ifdef ENABLE_STACKTRACE
    ,
    int numStackLevels, int stackInfosLength, /*Total length of char array for
                                                 all entries of stack levels*/
    int indicesLength,     /*=numStackLevels*3; used for convenience*/
    int *infoIndices,      /*Array of size 3*numStackLevels with indices to the
                              individual information pieces, for each level
                              infoIndices[level*3+0]=symName,
                              infoIndices[level*3+1]=lineOffset,
                              infoIndices[level*3+2]=fileModule*/
    const char *stackInfos /*All information pieces of all stack levels
                              concatenated, of length stackInfosLength*/
#endif
) {return 0;}

typedef int (*passLocationAcrossP) (
        MustParallelId pId,
        MustLocationId lId,
        const char* callName,
        int callNameLen,
        const void* codeptr,
        const char* fname,
        size_t fnameLen,
        const void* fbase,
#ifdef ENABLE_STACKTRACE
    int numStackLevels, int stackInfosLength, /*Total length of char array for
                                                 all entries of stack levels*/
    int indicesLength,      /*=numStackLevels*3; used for convenience*/
    int *infoIndices,       /*Array of size 3*numStackLevels with indices to the
                               individual information pieces, for each level
                               infoIndices[level*3+0]=symName,
                               infoIndices[level*3+1]=lineOffset,
                               infoIndices[level*3+2]=fileModule*/
    const char *stackInfos, /*All information pieces of all stack levels
                               concatenated, of length stackInfosLength*/
#endif
    int toPlaceId);

// handleNewMessage
inline int PhandleNewMessage (
		int msgId,
		int hasLocation,
		MustParallelId pId,
		MustLocationId lId,
		size_t fileId,
		int msgType,
		char* text,
		int textLen,
		int numReferences,
		MustParallelId *refPIds,
		MustLocationId *refLIds)
{return 0;}

typedef int (*handleNewMessageP) (int, int, MustParallelId, MustLocationId, size_t, int, char*, int, int, MustParallelId*, MustLocationId*);

// handleNewMessageReduced
inline int PhandleNewMessageReduced (
        int msgId,
        MustParallelId pId,
        MustLocationId lId,
        size_t fileId,
        int startRank,
        int stride,
        int count,
        int msgType,
        char* text,
        int textLen,
        int numReferences,
        MustParallelId *refPIds,
        MustLocationId *refLIds)
{return 0;}

typedef int (*handleNewMessageReducedP) (int, MustParallelId, MustLocationId, size_t, int, int, int, int, char*, int, int, MustParallelId*, MustLocationId*);

// finalizeNotify
inline int PfinalizeNotify () {return 0;}

typedef int (*finalizeNotifyP) ();

// finalizeNotify
inline int PfinalizeMUST () {return 0;}

typedef int (*finalizeMUSTP) ();


// TEST event for downwards communication: testDownComm 
/*inline int PtestDownComm (float value) {return 0;}
typedef int (*testDownCommP) (float value);*/

#endif /*BASE_API_H*/
