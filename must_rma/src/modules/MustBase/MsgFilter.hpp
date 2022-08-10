/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MUST_MSGFILTER_H
#define MUST_MSGFILTER_H

#include <algorithm>
#include <istream>
#include <memory>
#include <string>
#include <typeinfo>
#include <map>
#include <utility>
#include <vector>

#include "BaseIds.h"
#include "MustEnums.h"

namespace must {
namespace filter {

namespace detail {
/**
 * Checks if the string ends with the given suffix.
 *
 * @param str the string
 * @param end the suffix
 * @return true iff end is a suffix of str
 *
 * Note: Calls of this function should be replaced by std::string::ends_with() when migrating to
 * C++20
 */
auto ends_with(const std::string &str, const std::string &end) -> bool;
} // namespace detail

/**
 * Interface to pass the minimal necessary to filter.
 * @tparam CallstackInputIt Input iterator type over MustStackLevelInfo objects
 */
template <typename CallstackInputIt> class MsgInfo {
  public:
    /**
     * Get the ID of the message to match.
     * @return one of the must::MustMessageIdNames
     */
    virtual auto msg_id() const -> MustMessageIdNames = 0;

    /**
     * First element to iterate the call stack over.
     * @return iterator to the first element in the callstack
     */
    virtual auto stack_begin() const -> CallstackInputIt = 0;

    /**
     * Returns an iterator to the element following the last element of the
     * callstack.
     * @return Iterator to the first element in the callstack
     */
    virtual auto stack_end() const -> CallstackInputIt = 0;
};

/**
 * Parser for filterfiles.
 * It reads the filterfile line by line from an std::istream.
 */
class FilterfileParser {

    /**
     * Base class to represent an origin to match for.
     */
    struct OriginSpec {
        virtual ~OriginSpec() = default;
    };

    /**
     * Represents a complete valid message type filter rule consisting of the
     * message id and an one origin specification.
     */
    struct MessageTypeRule {
        /// The message id to filter for
        must::MustMessageIdNames msg_id;
        /// Convenience typedef
        using OriginSpecPtr = std::unique_ptr<OriginSpec>;
        /// Pointer to the origin specification
        OriginSpecPtr origin;

        /**
         * Constructor. It parses the tokens. Throws FilterParseError if the
         * tokenized line could not be parsed.
         */
        explicit MessageTypeRule(std::vector<std::string> &tokens);
    };

    /**
     * Empty Base parser exception class to disambiguate from std::exception.
     */
    class FilterParseError : public std::exception {};

    /**
     * Thrown if the filter line has an incorrect format.
     */
    class SyntaxError : public FilterParseError {
      public:
        auto what() const noexcept -> const char * override { return "syntax error"; }
    };

    /**
     * Thrown when the filter did not use either "fun", "src" or "obj" keywords.
     */
    class UnknownOriginKindError : public SyntaxError {
        std::string text;

      public:
        /**
         * Constructor.
         * @param loc_kind_text the wrong keyword
         */
        explicit UnknownOriginKindError(const std::string &loc_kind_text);
        auto what() const noexcept -> const char * override { return text.c_str(); }
    };

    /**
     * Thrown when a wildcard spec was expected.
     */
    class ValueError : public FilterParseError {
      public:
        auto what() const noexcept -> const char * override { return "value error"; }
    };

    /**
     * Thrown when the message type id is unknown.
     */
    class UnknownMessageIdError : public ValueError {
        std::string text;

      public:
        /**
         * Constructor.
         * @param msg_id_text the unknown message type id
         */
        explicit UnknownMessageIdError(const std::string &msg_id_text);
        auto what() const noexcept -> const char * override { return text.c_str(); }
    };

    /**
     * Generic error class to pass some error context to users of FilterfileParser
     */
    class ParseError {
        unsigned _lineno;
        std::string _msg;
        std::string _content;

      public:
        /**
         * Constructor.
         * @param lineno The line on which the error occurred
         * @param msg A descriptive error message
         * @param content The whole erroneous line
         */
        ParseError(unsigned lineno, std::string msg, std::string content)
            : _lineno{lineno}, _msg{std::move(msg)}, _content{std::move(content)} {}

        /**
         * Getter for the linenumber on which the error occurred.
         * @return the linenumber
         */
        auto lineno() const -> unsigned int { return _lineno; }

        /**
         * Getter for the descriptive error message.
         * @return the message
         */
        auto msg() const -> const std::string & { return _msg; };

        /**
         * Getter for erroneous line content.
         * @return the wrong filter line
         */
        auto content() const -> const std::string & { return _content; };
    };

    /**
     * Represents the kind of filter rule. Currently holds only one kind of
     * filter.
     */
    enum class FilterKind { unknown, messageType };

    /**
     * Helper function that translates strings to a FilterKind.
     * @param kind_text The name of the filter kind. E.g. "messageType"
     * @return the FilterKind
     */
    static auto determine_filter_kind(const std::string &kind_text) -> FilterKind {
        if (kind_text == "messageType") {
            return FilterKind::messageType;
        }
        return FilterKind::unknown;
    }

    using MustErrorString2IdMap = std::map<std::string, must::MustMessageIdNames>;
    /// Maps strings to their must::MessageIdNames enum representation.
    static const MustErrorString2IdMap id_map;

    /// Holds the parsed valid messageType filters
    std::vector<MessageTypeRule> _message_type_rules;

    /// Holds the errors that occurred during parsing.
    std::vector<ParseError> _errors{};

  public:
    /**
     * Reads and parses the linewise filter rules from the input stream.
     *
     * The method tries to continue to process the file on errors if possible
     * (Mostly the case on format errors.). So even while an error might have
     * occurred the user can still retrieve all other valid filter rules that
     * could be parsed successfully.
     *
     * @param input The stream to read from.
     * @return true, if all went well. false if an error occured.
     */
    auto parse(std::istream &input) -> bool;

    /**
     * Get all message type filter representations after parsing with #parse().
     * @return all rules (empty if called before parsing)
     */
    auto message_type_rules() const -> const std::vector<MessageTypeRule> & {
        return _message_type_rules;
    }

    /**
     * Gets the errors that occured during #parse().
     * @return a collection of errors (empty if none occurred)
     */
    auto errors() const -> const std::vector<ParseError> & { return _errors; };

    /**
     * Represents an origin specification that filters against a symbol. ("fun")
     */
    struct FunctionSpec : OriginSpec {
        std::string symbolname;
        explicit FunctionSpec(std::string symbolname) : symbolname{std::move(symbolname)} {}
    };

    /**
     * Represents an origin specification that filters against a filename.
     */
    struct FilenameSpec : OriginSpec {
        std::string filename;
        explicit FilenameSpec(std::string filename) : filename{std::move(filename)} {}
    };

    /**
     * Represents an origin specification that filters against a wildcard.
     */
    struct WildcardSpec : OriginSpec {};

    /**
     * Represents a filename based filter that was specified using the "src"
     * keyword.
     */
    using SourcenameSpec = FilenameSpec;

    /**
     * Represents a filename based filter that was specified using the "obj"
     * keyword.
     */
    using ObjectnameSpec = FilenameSpec;
};

/**
 * Matches messages against filter rules. Used to filter messages based on the
 * filterrules parsed with FilterfileParser.
 *
 * @tparam CallstackInputIt Input iterator type over MustStackLevelInfo objects
 */
template <typename CallstackInputIt> class MsgFilter {
    /**
     * Interface to match against a origin of the call.
     */
    class OriginMatcher {
      public:
        virtual ~OriginMatcher() = default;

        /**
         * Matches against a MustStackLevelInfo object
         * @param level the call stack level to match against
         * @return true if the stack level matches, false otherwise
         */
        virtual auto match(const MustStackLevelInfo &level) const -> bool = 0;

        /**
         * Matches against a call stack.
         * @param first the first element of the call stack.
         * @param last points to the past-the-end element of the call stack.
         * @return true if any level of the call stack matches. false if none
         *         matches (or if the stack is empty).
         */
        virtual auto match(CallstackInputIt first, CallstackInputIt last) const -> bool {
            return std::any_of(first, last, [this](const MustStackLevelInfo &level) -> bool {
                return this->match(level);
            });
        }
    };

    /**
     * Matches against any stack level. Corresponds to a wildcard filter.
     */
    class AnywhereMatcher : public OriginMatcher {
      public:
        /**
         * Matches always.
         * @param level the call stack level
         * @return always true
         */
        auto match(const MustStackLevelInfo &level) const -> bool override { return true; }

        /**
         * Matches always.
         * @param first the first element of the call stack.
         * @param last points to the past-the-end element of the call stack.
         * @return always true
         */
        auto match(CallstackInputIt first, CallstackInputIt last) const -> bool override {
            return true;
        };
    };

    /**
     * Matches against symbol names.
     */
    class FunctionMatcher : public OriginMatcher {
        std::string function_name;

      public:
        /**
         * Constructor.
         * @param name symbol name to match against
         */
        explicit FunctionMatcher(std::string name) : function_name{std::move(name)} {}

        /**
         * Converting constructor.
         * @param spec the FilterfileParser::FunctionSpec that represents the symbol
         */
        explicit FunctionMatcher(const FilterfileParser::FunctionSpec &spec)
            : FunctionMatcher{spec.symbolname} {}

        /**
         * Matches against the symbol name of the call stack level.
         * @param level the callstack level to match against
         * @return true, if the symbol of the stackframe matches. false otherwise.
         */
        auto match(const MustStackLevelInfo &level) const -> bool override {
            return level.symName == function_name;
        }
    };

    /**
     * Matches against file names.
     */
    class FilenameMatcher : public OriginMatcher {
        std::string file_name;

      public:
        /**
         * Constructor.
         * @param name file name to match against
         */
        explicit FilenameMatcher(std::string name) : file_name{std::move(name)} {}

        /**
         * Converting constructor.
         * @param spec the FilterfileParser::FilenameSpec that represents file
         */
        explicit FilenameMatcher(const FilterfileParser::FilenameSpec &spec)
            : FilenameMatcher{spec.filename} {}

        /**
         * Matches against the file name of the call stack level.
         * @param level the callstack level to match against
         * @return true, if the symbol of the filename matches. false otherwise.
         */
        auto match(const MustStackLevelInfo &level) const -> bool override {
            const auto matcher_path_len = file_name.size();
            const auto stacktrace_path_len = level.fileModule.size();
            return detail::ends_with(level.fileModule, file_name) &&
                   (matcher_path_len == stacktrace_path_len // the paths are actually equal
                    || level.fileModule[stacktrace_path_len - matcher_path_len - 1] == '/');
        }
    };

    /**
     * Custom container for a collection of OriginMatchers to match against.
     */
    class OriginRules {
        using OriginMatcherPtr = std::unique_ptr<OriginMatcher>;
        std::vector<OriginMatcherPtr> locations{};
        bool has_already_wildcard = false;

      public:
        /**
         * Add an OriginMatcher to the collection.
         * @param loc the OriginMatcher to add
         */
        auto push_back(OriginMatcherPtr &&loc) -> void {
            if (!has_already_wildcard) {
                // temporary ref to avoid warning on unexpected side effects of typeid
                auto &r = *loc.get();
                if (typeid(r) == typeid(AnywhereMatcher)) {
                    // Throw away all already known Matchers. This one will always hit.
                    locations.clear();
                    has_already_wildcard = true;
                }
                locations.push_back(std::move(loc));
            }
        }

        /**
         * Matches against a whole call stack.
         * @param first the first element of the call stack.
         * @param last points to the past-the-end element of the call stack.
         * @return true if any level of the call stack matches. false if none
         *         matches (or if the stack is empty).
         */
        auto match(CallstackInputIt first, CallstackInputIt last) const -> bool {
            return std::any_of(locations.cbegin(), locations.cend(),
                               [&](const OriginMatcherPtr &matcher) -> bool {
                                   return matcher->match(first, last);
                               });
        }
    };

    std::map<MustMessageIdNames, OriginRules> filtered_ids{};

  public:
    /**
     * Default constructor.
     */
    MsgFilter<CallstackInputIt>() = default;

    /**
     * Reads in the parsed filter rules from the FilterFileParser.
     * @param parser the parser containing the filter rules.
     */
    explicit MsgFilter<CallstackInputIt>(const FilterfileParser &parser) {
        for (const auto &rule : parser.message_type_rules()) {
            const auto &origin_spec = *rule.origin;

            const auto &concrete_spec_type = typeid(origin_spec);
            OriginMatcher *matcher = nullptr;
            if (concrete_spec_type == typeid(FilterfileParser::WildcardSpec)) {
                matcher = new AnywhereMatcher{};
            } else if (concrete_spec_type == typeid(FilterfileParser::FunctionSpec)) {
                const auto &spec =
                    dynamic_cast<const FilterfileParser::FunctionSpec &>(origin_spec);
                matcher = new FunctionMatcher{spec};
            } else if (concrete_spec_type == typeid(FilterfileParser::SourcenameSpec)) {
                const auto &spec =
                    dynamic_cast<const FilterfileParser::SourcenameSpec &>(origin_spec);
                matcher = new FilenameMatcher{spec};
            } else if (concrete_spec_type == typeid(FilterfileParser::ObjectnameSpec)) {
                const auto &spec =
                    dynamic_cast<const FilterfileParser::ObjectnameSpec &>(origin_spec);
                matcher = new FilenameMatcher{spec};
            }

            filtered_ids[rule.msg_id].push_back(std::unique_ptr<OriginMatcher>{matcher});
        }
    }

    /**
     * Match the MsgInfo object against the filters.
     * @param info the context of the message to match against
     * @return true, if the filter matches. false, otherwise.
     */
    auto match(const MsgInfo<CallstackInputIt> &info) const -> bool {
        auto locationrules = filtered_ids.find(info.msg_id());
        if (locationrules != filtered_ids.end()) {
            return locationrules->second.match(info.stack_begin(), info.stack_end());
        }
        return false;
    }
};

} // namespace filter
} // namespace must

#endif
