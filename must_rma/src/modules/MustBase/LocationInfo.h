/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file LocationInfo.h
 *       Struct and helpers for location id information.
 *
 *  @date 24.04.2014
 *  @author Tobias Hilbrich
 */

#include "BaseIds.h"
#include "mustConfig.h"
#ifdef ENABLE_STACKTRACE
#include <list>
#endif
#include <vector>

#ifndef LOCATIONINFO_H
#define LOCATIONINFO_H

namespace must
{
    /**
     * Information for a location id.
     */
#ifdef ENABLE_STACKTRACE
template <typename T>
#endif
struct LocationInfoImpl {
  std::string callName; /**< Name of the call that created the record.*/
  const void* codeptr; /**< Pointer of the source location. */
  std::string fname;   /**< Name of the location's source object. */
  const void* fbase;   /**< Base memory address of the source location. */
#ifdef ENABLE_STACKTRACE
  std::list<T> stack;
#endif
};

struct LocationInfo : LocationInfoImpl
#ifdef ENABLE_STACKTRACE
                      <MustStackLevelInfo>
#endif
{
};

#ifdef ENABLE_STACKTRACE
template <typename T>
inline bool operator<(const LocationInfoImpl<T> &a,
                      const LocationInfoImpl<T> &b) {
  // Criteria A: callName
  if (a.callName < b.callName)
    return true;

  if (a.callName != b.callName)
    return false;

  // Criteria B: stack depth
  if (a.stack.size() < b.stack.size())
    return true;

  if (a.stack.size() != b.stack.size())
    return false;

  // Critieria C: stack->symName && lineOffset
  typename std::list<T>::const_iterator aIter, bIter;
  for (aIter = a.stack.begin(), bIter = b.stack.begin(); aIter != a.stack.end();
       aIter++, bIter++) {
    if (!(*aIter == *bIter))
      return *aIter < *bIter;
  }

  // We consider this equal if all stack symbol names and the call name are
  // equal (We do not look at file or offsets or module or line, this should
  //usually be ok)
  return false;
}
#endif

} /*namespace must*/

#endif /*LOCATIONINFO_H*/
