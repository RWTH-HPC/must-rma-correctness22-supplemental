/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TSANMESSAGES_H
#define TSANMESSAGES_H

/**
 * @file TSanMessages.h
 *       @see MUST::TSanMessages.
 *
 *  @date 23.11.2017
 *  @author Felix Dommes
 */

#include "BaseApi.h"
#include "I_CreateMessage.h"
#include "I_InitLocationId.h"
#include "I_InitParallelId.h"
#include "I_TSanMessages.h"
#include "ModuleBase.h"

namespace __tsan {
class ReportDesc;
}

namespace must
{
    /**
     * Adapter to pipe TSan messages into MUST-messages
     */
    class TSanMessages : public gti::ModuleBase<TSanMessages, I_TSanMessages>
    {
    public:
        /**
         * Constructor.
         * @param instanceName name of this module instance.
         */
      explicit TSanMessages(const char *instanceName);

      /**
       * Destructor.
       */
      ~TSanMessages() override;

      /**
       * Creates messages from TSan reports.
       *
       * @param report pointer to the Thread Sanitizer report
       * @return see gti::GTI_ANALYSIS_RETURN.
       */
      gti::GTI_ANALYSIS_RETURN tsanReport(const __tsan::ReportDesc *report);

      gti::GTI_ANALYSIS_RETURN fini() override;

    private:
      I_CreateMessage *myLogger;
      I_InitParallelId *myPIdInit;
      I_InitLocationId *myLIdInit;
      handleNewLocationP myNewLocFunc;
    };
} /*namespace MUST*/

#endif /*TSANMESSAGES_H*/
