/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file CreateMessage.h
 *       Implementation for I_CreateMessage.
 *
 *  @date 19.01.2011
 *  @author Tobias Hilbrich
 */

#ifndef CREATEMESSAGE_H
#define CREATEMESSAGE_H

#include <list>
#include <map>

#include "ModuleBase.h"

#include "I_CreateMessage.h"
#include "InitLocationId.h"
#include "MsgFilter.hpp"
#include "MustEnums.h"

class I_LocationAnalysis;

namespace must
{
#ifdef ENABLE_STACKTRACE
/// Type alias for MUST's stacktrace type
using MustStack_t = decltype(LocationInfo::stack);
#else
/// Type alias for the dummy stacktrace type
using MustStack_t = std::array<MustStackLevelInfo, 0>;
#endif
/**
 * Implementation for the I_CreateMessage interface.
 * This is used to create new logging events
 */
class CreateMessage : public gti::ModuleBase<CreateMessage, I_CreateMessage> {
    /// filter based on the filterfile
    filter::MsgFilter<MustStack_t::const_iterator> filter;

  public:
    /**
     * Constructor.
     * @param instanceName name of this module instance.
     */
    CreateMessage(const char *instanceName);

    /**
      * Destructor.
      */
    virtual ~CreateMessage(void);

    /**
     * @see I_MessageLogger::createMessage.
     */
    gti::GTI_ANALYSIS_RETURN
    createMessage(int msgId, MustParallelId pId, MustLocationId lId, MustMessageType msgType,
                  std::string text,
                  std::list<std::pair<MustParallelId, MustLocationId>> refLocations);

    /**
     * @see I_MessageLogger::createMessage.
     */
    gti::GTI_ANALYSIS_RETURN
    createMessage(int msgId, MustMessageType msgType, std::string text,
                  std::list<std::pair<MustParallelId, MustLocationId>> refLocations);

    /**
     * @see I_CreateMessage::changeFileId
     */
    GTI_ANALYSIS_RETURN changeFileId(size_t fileId);

  protected:
    /**
      * The fileId used for messages being created.
      *
      * This variable stores the fileId being used for new messages. Its
      * initial value is `0` and will be updated by @ref changeFileId, if the
      * message logging file changes.
      */
    size_t myFileId;

    /**
     * Internal function to propagate messages with location along with
     * global messages.
     * @see I_MessageLogger::createMessage.
     */
    gti::GTI_ANALYSIS_RETURN
    createMessage(int msgId, int hasLocation, MustParallelId pId, MustLocationId lId,
                  MustMessageType msgType, std::string text,
                  std::list<std::pair<MustParallelId, MustLocationId>> &refLocations);

    class GInfo {
      public:
        int msgId;
        MustMessageType msgType;

        bool operator<(const GInfo &other) const;
    };

    class LInfo {
      public:
        int msgId;
        MustMessageType msgType;
        MustParallelId pId;
        MustLocationId lId;

        bool operator<(const LInfo &other) const;
    };

    std::map<GInfo, int> myGMsgs; /**< Count of global messages of a certain type,
                                     used to filter.*/
    std::map<LInfo, int> myLMsgs; /**< Count of local messages of a certain type, used to filter.*/

    /// Used to obtain the stacktrace for filtering
    I_LocationAnalysis *myLIdModule;

}; /*class CreateMessage */
} /*namespace MUST*/

#endif /*CREATEMESSAGE_H*/
