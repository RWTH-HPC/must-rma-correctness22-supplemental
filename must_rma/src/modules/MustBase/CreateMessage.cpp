/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file CreateMessage.cpp
 *       @see MUST::CreateMessage.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "GtiMacros.h"
#include "BaseApi.h"
#include "PrefixedOstream.hpp"

#include "CreateMessage.h"
#include <fstream>
#include <string>
#include <vector>

#include <algorithm>
#include <iostream>

#include "I_LocationAnalysis.h"
#include "LocationInfo.h"
#include "MsgFilter.hpp"

using namespace gti;
using namespace must;

// clang-format off
mGET_INSTANCE_FUNCTION(CreateMessage)
mFREE_INSTANCE_FUNCTION(CreateMessage)
mPNMPI_REGISTRATIONPOINT_FUNCTION(CreateMessage)
    // clang-format on

    //=============================
    // GInfo::operator <
    //=============================
    bool CreateMessage::GInfo::operator<(const GInfo &other) const {
    if (msgId < other.msgId)
        return 1;
    if (msgId > other.msgId)
        return 0;

    if (msgType < other.msgType)
        return 1;

    return 0;
}

//=============================
// LInfo::operator <
//=============================
bool CreateMessage::LInfo::operator < (const LInfo& other) const
{
    if (msgId < other.msgId)
        return 1;
    if (msgId > other.msgId)
        return 0;

    if (msgType < other.msgType)
        return 1;
    if (msgType > other.msgType)
        return 0;

    if (pId < other.pId)
        return 1;
    if (pId > other.pId)
        return 0;

    if ((lId & 0x00000000FFFFFFFF) < (other.lId & 0x00000000FFFFFFFF)) //Kill the occurence count, that one is of no interest here!
        return 1;

    return 0;
}

template <typename CallstackIt> static auto parse_filterfile() -> filter::MsgFilter<CallstackIt> {
    const auto *filterfile = std::getenv("MUST_FILTER_FILE");
    if (filterfile != nullptr) {
        std::ifstream file_in(filterfile);
        if (!file_in) {
            must::cerr << "Warning: Could not open the filterfile \"" << filterfile << "\""
                       << std::endl;
        } else {
            auto parser = filter::FilterfileParser{};
            auto success = parser.parse(file_in);
            if (!success) {
                // errors occurred during parsing. get them and emit warnings.
                for (const auto &err : parser.errors()) {
                    auto buf = std::stringstream{};
                    must::cerr << "Error while parsing the filter rule in the file \"" << filterfile
                               << "\""
                               << " at line " << err.lineno() << ": " << err.msg();
                    /* This creates new instances recursively:
                    createMessage(MustMessageIdNames::MUST_MESSAGE_NO_ERROR,
                                  MustMessageType::MustWarningMessage,
                                  buf.str(),
                                  std::list <std::pair<MustParallelId,
                    MustLocationId>>{});
                                  */
                }
            }
            // check for stream errors that occured during parsing
            if (file_in.bad()) {
                must::cerr << "Read error for the filterfile \"" << filterfile << "\"";
                /*
                createMessage(MustMessageIdNames::MUST_MESSAGE_NO_ERROR,
                              MustMessageType::MustWarningMessage,
                              buf.str(),
                              std::list <std::pair<MustParallelId,
                MustLocationId>>{});*/
            }
            return filter::MsgFilter<CallstackIt>{parser};
        }
    }
    return filter::MsgFilter<CallstackIt>{};
}

//=============================

//=============================
// Destructor
//=============================
CreateMessage::~CreateMessage (void)
{
    if (myLIdModule)
        destroySubModuleInstance ((I_Module*) myLIdModule);
    myLIdModule = NULL;
}

// Constructor
//=============================
CreateMessage::CreateMessage (const char* instanceName)
    : gti::ModuleBase<CreateMessage, I_CreateMessage> (instanceName),
      myGMsgs (),
      myLMsgs (),
      myFileId (0)
{
    //create sub modules
    const auto subModInstances = createSubModuleInstances();

    // save sub modules
    myLIdModule = (I_LocationAnalysis*)subModInstances[0];

    filter = parse_filterfile<MustStack_t::const_iterator>();
}

//=============================
// createMessage
//=============================
GTI_ANALYSIS_RETURN
CreateMessage::createMessage(int msgId, MustMessageType msgType, std::string text,
                             std::list<std::pair<MustParallelId, MustLocationId>> refLocations)

{
    //Determine whether we filter out this message
    const auto info = GInfo{.msgId = msgId, .msgType = msgType};
    auto pos = myGMsgs.find(info);
    if (pos != myGMsgs.end())
    {
        pos->second = pos->second +1;
        return GTI_ANALYSIS_SUCCESS;
    }

    //We do not filter, create the message
    myGMsgs.insert(std::make_pair (info, 1));
    return createMessage (msgId, 0, 0, 0, msgType, text, refLocations);
}

//=============================
// createMessage
//=============================
GTI_ANALYSIS_RETURN
CreateMessage::createMessage(int msgId, MustParallelId pId, MustLocationId lId,
                             MustMessageType msgType, std::string text,
                             std::list<std::pair<MustParallelId, MustLocationId>> refLocations) {
    //Determine whether we filter out this message
    auto info = LInfo{.msgId = msgId, .msgType = msgType, .pId = pId, .lId = lId};
    auto pos = myLMsgs.find(info);
    if (pos != myLMsgs.end())
    {
        pos->second = pos->second +1;
        return GTI_ANALYSIS_SUCCESS;
    }

    //We do not filter, create the message
    myLMsgs.insert(std::make_pair (info, 1));
    return createMessage(msgId, 1, pId, lId, msgType, text, refLocations);
}

/**
 * Small class that implements the filter::MsgInfo interface. Used to pass
 * matching context to the filter.
 */
class MsgInfoImpl : public filter::MsgInfo<MustStack_t::const_iterator> {
    using Callstack = MustStack_t;
    MustMessageIdNames id;
    const Callstack &stack;

  public:
    /**
     * Constructor.
     * @param messageTypeId The message type id to match against
     * @param stack The callstack to match against.
     */
    MsgInfoImpl(MustMessageIdNames messageTypeId, const Callstack &stack)
        : id{messageTypeId}, stack{stack} {}
    auto msg_id() const -> MustMessageIdNames override { return id; }
    auto stack_begin() const -> Callstack::const_iterator override { return stack.cbegin(); }
    auto stack_end() const -> Callstack::const_iterator override { return stack.cend(); }
};

//=============================
// createMessage
//=============================
GTI_ANALYSIS_RETURN
CreateMessage::createMessage(int msgId, int hasLocation, MustParallelId pId, MustLocationId lId,
                             MustMessageType msgType, std::string text,
                             std::list<std::pair<MustParallelId, MustLocationId>> &refLocations) {
    // Filter to not show the Errors/Warnings
    LocationInfo info = myLIdModule->getInfoForId(pId, lId);
#ifdef ENABLE_STACKTRACE
    const auto &stack = info.stack;
#else
    // Use empty dummy stack. Only wildcard matching will work this way.
    const auto stack = MustStack_t{};
#endif
    const auto message_info = MsgInfoImpl{static_cast<MustMessageIdNames>(msgId), stack};
    if (filter.match(message_info)) {
        return GTI_ANALYSIS_SUCCESS;
    }

    // Call handleNewMessage
    handleNewMessageP fP = nullptr;
    if (getWrapperFunction("handleNewMessage", (GTI_Fct_t *)&fP) == GTI_SUCCESS) {
        auto refPIds = std::vector<MustParallelId>{};
        refPIds.reserve(refLocations.size());
        auto refLIds = std::vector<MustLocationId>{};
        refLIds.reserve(refLocations.size());
        for (const auto &pair : refLocations) {
            refPIds.push_back(pair.first);
            refLIds.push_back(pair.second);
        }

        // We cast the cons of the char away here, we can't use const char* in the definintion of
        // the API call, but it will not modify the text
        (*fP)(msgId, hasLocation, pId, lId, myFileId, static_cast<int>(msgType), const_cast<char *>(text.c_str()),
              static_cast<int>(text.length() + 1), static_cast<int>(refLocations.size()),
              refPIds.data(), refLIds.data());
    } else {
        must::cout << "ERROR: failed to get \"handleNewMessage\" function pointer from wrapper, "
                      "load the MUST base API, logging is not possible as a result!"
                   << std::endl;
    }

    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// changeFileId
//=============================
GTI_ANALYSIS_RETURN CreateMessage::changeFileId (size_t fileId)
{
    myFileId = fileId;
    return gti::GTI_ANALYSIS_SUCCESS;
}

/*EOF*/
