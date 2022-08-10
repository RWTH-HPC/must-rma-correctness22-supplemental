/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TSanMessages.cpp
 *       @see MUST::TSanMessages.
 *
 *  @date 23.11.2017
 *  @author Joachim Protze, Felix Dommes
 */

#include "TSanMessages.h"

#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "BaseIds.h"
#include "GtiMacros.h"
#include "GtiTypes.h"
#include "I_Module.h"
#include "MustEnums.h"
#include "PrefixedOstream.hpp"

#include "tsan_report_desc.hpp"

using namespace std;
using namespace gti;
using namespace must;

mGET_INSTANCE_FUNCTION(TSanMessages)
mFREE_INSTANCE_FUNCTION(TSanMessages)
mPNMPI_REGISTRATIONPOINT_FUNCTION(TSanMessages)

static bool finalized = false;

namespace {

/**
 * Builds up a stack trace string from those given by the report.
 *
 * This constructs a stacktrace in a format that can be passed to the MUST
 * message mechanism. The format is built up as one big non-zero-terminated
 * string. It consists of the concatenated symbolname, filename and linenumber
 * for each stack level.
 * The `indices` point to the last character of each component in `locstrings`,
 * such that indices[i] is the offset in locStrings to the last character of
 * the i-th substring.
 *
 * @param stack the stack trace
 * @param locStrings[out] whole concatenated string
 * @param indices[out] the indices that mark the end of each substring of
 *                     locStrings
 * @param stack_depth[out] the stack strace's height
 */
void build_stacktrace(const __tsan::ReportStack *const stack,
                      std::string &locString, std::vector<int> &indices,
                      int &stack_depth) {
  stack_depth = 0;
  locString = "";
  indices.clear();
  for (auto *pFrame = stack->frames; pFrame != nullptr; pFrame = pFrame->next) {
    stack_depth += 1;
    // symbol name
    if (!pFrame->info.function)
      continue;
    locString.append(pFrame->info.function);
    indices.emplace_back(locString.length() - 1);
    // file/module name
    if (pFrame->info.file != nullptr) {
      locString.append(pFrame->info.file);
    } else if (pFrame->info.module != nullptr) {
      locString.append(pFrame->info.module);
    } else {
      continue;
    }
    indices.emplace_back(locString.length() - 1);
    // line/module offset number
    if (pFrame->info.line != 0) {
      locString.append(to_string(pFrame->info.line));
    } else {
      auto tmp = std::stringstream{};
      tmp << "0x" << std::hex << pFrame->info.module_offset;
      locString.append(tmp.str());
    }
    indices.emplace_back(locString.length() - 1);
  }
}

/**
 * Formats a message in MUST style from the Thread Sanitizer's report.
 * @param report the report provided by Thread Sanitizer
 * @return the formatted message string
 */
auto format(const __tsan::ReportDesc *const report) -> std::string {
  stringstream msg;
  const auto *lastMop = (*report->mops.begin_);
  msg << "Data race between a " << (lastMop->write ? "write" : "read")
      << " of size " << lastMop->size << " at "
      << lastMop->stack->frames->info.function << "@1";
  for (auto **it = report->mops.begin_ + 1; it != report->mops.end_; ++it) {
    auto *pMop = *it;
    if (it == report->mops.end_ - 1) {
      msg << " and ";
    } else {
      msg << ", ";
    }
    msg << "a previous " << (pMop->write ? "write" : "read") << " of size "
        << pMop->size << " at " << pMop->stack->frames->info.function << "@"
        << distance(report->mops.begin_, it) + 1;
  }
  msg << ".";
  return msg.str();
}
} // namespace

namespace __tsan
{
/**
 * Called by OnReport() in OnReportLoader.cpp when Thread Sanitizer emits a
 * report.
 *
 * It forwards the report the the TSanMessages instance and prevents thread
 * sanitizer to print its own message to the console.
 */
extern "C" bool OnReport(const __tsan::ReportDesc *rep, bool _suppressed) {
  // we only support data races
  switch (rep->typ) {
  case __tsan::ReportTypeRace:
    break;
  default:
    return _suppressed;
  }

  // forward data-race report
  if (!finalized) {
    TSanMessages *tsanMessages = TSanMessages::getInstance("");
    tsanMessages->tsanReport(rep);
  }

  // returning true prevents tsan from printing its own report to stdout
  // return _suppressed;
  return true;
}
} /* namespace __tsan */

//=============================
// Constructor
//=============================
TSanMessages::TSanMessages (const char* instanceName)
    : gti::ModuleBase<TSanMessages, I_TSanMessages> (instanceName)
{
    //create sub modules
    vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();

    //handle sub modules
#define NUM_SUBMODULES 3
    if (subModInstances.size() < NUM_SUBMODULES)
    {
        must::cerr << "Module has not enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << endl;
        assert (0);
    }
    if (subModInstances.size() > NUM_SUBMODULES)
    {
        for (vector<I_Module*>::size_type i = NUM_SUBMODULES; i < subModInstances.size(); i++)
            destroySubModuleInstance (subModInstances[i]);
    }

    myLogger = (I_CreateMessage*) subModInstances[0];
    myPIdInit = (I_InitParallelId*) subModInstances[1];
    myLIdInit = (I_InitLocationId*) subModInstances[2];

	//get handleNewLocation function
	getWrapperFunction("handleNewLocation", (GTI_Fct_t*)&myNewLocFunc);
}

//=============================
// Destructor
//=============================
TSanMessages::~TSanMessages ()
{
  if (myLogger != nullptr) {
    destroySubModuleInstance((I_Module *)myLogger);
    myLogger = nullptr;
  }
  if (myPIdInit != nullptr) {
    destroySubModuleInstance((I_Module *)myPIdInit);
    myPIdInit = nullptr;
  }
  if (myLIdInit != nullptr) {
    destroySubModuleInstance((I_Module *)myLIdInit);
    myLIdInit = nullptr;
  }
}

//=============================
// tsanReport
//=============================
auto TSanMessages::tsanReport(const __tsan::ReportDesc * const report) -> GTI_ANALYSIS_RETURN {
  // extract locations
  auto locations = std::vector<std::string>{};
  for (auto **it = report->mops.begin_; it != report->mops.end_; ++it) {
    auto *pMop = *it;
    locations.emplace_back(pMop->stack->frames->info.function);
  }

#ifdef ENABLE_STACKTRACE
  // build the stack info from the report
  auto locStrings = std::vector<std::string>{};
  auto indices = std::vector<std::vector<int>>{};
  auto stackLevels = std::vector<int>{};
  for (auto **it = report->mops.begin_; it != report->mops.end_; ++it) {
    auto stack_string = std::string{};
    auto stack_indices = std::vector<int>{};
    int levels = 0;
    build_stacktrace((*it)->stack, stack_string, stack_indices, levels);
    locStrings.push_back(stack_string);
    indices.push_back(stack_indices);
    stackLevels.push_back(levels);
  }
#endif

  auto lId = std::vector<MustParallelId>{};
  for (auto i = 0U; i < locations.size(); ++i) {
    MustLocationId location_id = 0;
    myLIdInit->getUniqueLocationId(&location_id);
    lId.push_back(location_id);
  }

  // obtain IDs and register the locations
  MustParallelId pId = 0;
  myPIdInit->init(&pId);

  for (std::size_t i = 0; i < locations.size(); i++)
  {
    auto result = (*myNewLocFunc)(
        pId,
        lId[i],
        locations[i].c_str(),
        locations[i].length() + 1,
        NULL,
        "\0",
        1,
        NULL
#ifdef ENABLE_STACKTRACE
        ,
        stackLevels[i],
        locStrings[i].length() + 1, // the length of all concatenated stack strings
        indices[i].size(), // the number of indices (this is claimed to be convenient)
        indices[i].data(),
        locStrings[i].c_str() //should only be read access
#endif
        );
    assert(result == GTI_SUCCESS);
  }

  // create the reflist
  auto refList = list< pair<MustParallelId, MustLocationId> >{};
  for (const auto id : lId) {
    refList.emplace_back(make_pair(pId, id));
  };

  // format and create some nice message
  auto msg = format(report);

  myLogger->createMessage(MUST_WARNING_DATARACE, pId, lId[0], MustErrorMessage,
                          msg, refList);
  return GTI_ANALYSIS_SUCCESS;
}

GTI_ANALYSIS_RETURN TSanMessages::fini()
{
  finalized = true;
  return GTI_ANALYSIS_SUCCESS;
}
