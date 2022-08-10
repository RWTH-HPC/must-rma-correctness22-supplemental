/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MsgLoggerDdt.h
 *       @see MUST::MsgLoggerDdt.
 *
 *  @date 11.10.2012
 *  @author Joachim Protze
 */

#include "ModuleBase.h"
#include "I_MessageLogger.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"

#ifndef MSGLOGGERDDT_H
#define MSGLOGGERDDT_H

using namespace gti;

extern "C" void myDdtBreakpointFunctionError(const char* text);
extern "C" void myDdtBreakpointFunctionWarning(const char* text);
extern "C" void myDdtBreakpointFunctionInfo(const char* text);

namespace must
{
  /**
     * Implementation of I_MessageLogger that prints all logged events to
     * must::cout.
     */
    class MsgLoggerDdt : public gti::ModuleBase<MsgLoggerDdt, I_MessageLoggerCId>
    {
    public:
      /**
       * Constructor.
       * @param instanceName name of this module instance.
       */
      MsgLoggerDdt (const char* instanceName);

      /**
       * Destructor.
       */
      virtual ~MsgLoggerDdt (void);

      /**
       * @see I_MessageLogger::log.
       */
      GTI_ANALYSIS_RETURN log (
          int msgId,
          int hasLocation,
          uint64_t pId,
          uint64_t lId,
          size_t fileId,
          int msgType,
          char *text,
          int textLen,
          int numReferences,
          uint64_t* refPIds,
          uint64_t* refLIds,
          I_ChannelId *cId
      );

      /**
       * @see I_MessageLogger::logStrided.
       */
      GTI_ANALYSIS_RETURN logStrided (
              int msgId,
              uint64_t pId,
              uint64_t lId,
              size_t fileId,
              int startRank,
              int stride,
              int count,
              int msgType,
              char *text,
              int textLen,
              int numReferences,
              uint64_t* refPIds,
              uint64_t* refLIds,
              I_ChannelId *cId
      );

    protected:
      I_ParallelIdAnalysis *myPIdModule;
      I_LocationAnalysis *myLIdModule;
    }; /*class MsgLoggerDdt */
} /*namespace MUST*/

#endif /*MSGLOGGERDDT_H*/
