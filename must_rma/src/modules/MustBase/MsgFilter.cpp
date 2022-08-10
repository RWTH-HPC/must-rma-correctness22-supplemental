/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MsgFilter.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <map>
#include <utility>
#include <vector>

namespace {
/**
 * Splits the string at the specified character.
 * @param s the string to split
 * @param c the delimiter
 * @return the split parts without the delimiter
 */
auto split(std::string s, char c) -> std::vector<std::string> {
    auto vec = std::vector<std::string>{};
    for (std::string::size_type i = 0; i < s.size(); i++) {
        if (s[i] == c) {
            vec.push_back(s.substr(0, i));
            s = s.substr(i + 1, s.size() - i);
            i = 0;
        }
    }
    vec.push_back(s);
    return vec;
}

/// literal of (hopefully all) whitespace characters
const char *const WHITESPACE = " \n\r\t\f\v";

/**
 * Remove all whitespace at the beginning of the string.
 * @param s the input string
 * @return a copy of s without preceding whitespace
 */
auto ltrim(const std::string &s) -> std::string {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

/**
 * Remove all whitespace at the end of the string.
 * @param s the input string
 * @return a copy of s without trailing whitespace
 */
auto rtrim(const std::string &s) -> std::string {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

/**
 * Remove all whitespace at the begin and end of the string.
 * @param s the input string
 * @return a copy of s without surrounding whitespace
 */
auto trim(const std::string &s) -> std::string { return rtrim(ltrim(s)); }

/**
 * Splits with ':' as delimiter and trims the parts.
 * @param line the input
 * @return the copied parts
 */
auto tokenize(std::string line) -> std::vector<std::string> {
    auto parts = split(std::move(line), ':');
    for (auto &part : parts) {
        part = trim(part);
    }
    return parts;
}
} // namespace

namespace must {
namespace filter {

namespace detail {
auto ends_with(const std::string &str, const std::string &end) -> bool {
    return str.size() >= end.size() &&
           str.compare(str.size() - end.size(), std::string::npos, end) == 0;
}
} // namespace detail

#define map_entry_macro(name) {#name, name},
const FilterfileParser::MustErrorString2IdMap FilterfileParser::id_map = MustErrorString2IdMap{
    {"MUST_MESSAGE_NO_ERROR", MUST_MESSAGE_NO_ERROR},
    // Errors
    FOREACH_MUST_ERRORS(map_entry_macro)
    // Warnings
    FOREACH_MUST_WARNING(map_entry_macro)
    // Informations
    FOREACH_MUST_INFO(map_entry_macro){"MUST_LAST_MESSAGE_ID_NAME", MUST_LAST_MESSAGE_ID_NAME}};
#undef map_entry_macro

auto FilterfileParser::parse(std::istream &input) -> bool {
    auto lineno = 0U;
    auto line = std::string{};
    while (std::getline(input, line)) {
        lineno++;
        line = trim(line);

        if (line.empty() || line[0] == '#') {
            // ignore empty lines and comments
            continue;
        }

        auto tokens = tokenize(line);
        assert(!tokens.empty());
        auto kind = determine_filter_kind(tokens[0]);
        switch (kind) {
        case FilterKind::messageType:
            try {
                _message_type_rules.emplace_back(tokens);
            } catch (const FilterParseError &err) {
                _errors.emplace_back(lineno, err.what(), line);
            }
            break;
        case FilterKind::unknown:
        default:
            auto msg = std::stringstream{};
            msg << "unknown filter kind: \"" << tokens[0] << "\"";
            _errors.emplace_back(lineno, msg.str(), line);
            break;
        }
    }
    return input.bad() || _errors.empty();
}

FilterfileParser::MessageTypeRule::MessageTypeRule(std::vector<std::string> &tokens) {
    if (tokens.size() < 3) {
        throw SyntaxError{};
    }

    // get the message type id
    const auto id_mapping_it = id_map.find(tokens[1]);
    if (id_mapping_it == id_map.end()) {
        throw UnknownMessageIdError{tokens[1]};
    }
    msg_id = id_mapping_it->second;

    if (tokens.size() == 3) {
        // we expect a wildcard rule here
        if (tokens[2] != "*") {
            throw SyntaxError{};
        }
        origin = OriginSpecPtr{new WildcardSpec{}};
    } else if (tokens[2] == "fun") {
        origin = OriginSpecPtr{new FunctionSpec{tokens[3]}};
    } else if (tokens[2] == "src") {
        origin = OriginSpecPtr{new SourcenameSpec{tokens[3]}};
    } else if (tokens[2] == "obj") {
        origin = OriginSpecPtr{new ObjectnameSpec{tokens[3]}};
    } else {
        // no [fun|src|obj] -> Syntax error
        throw UnknownOriginKindError{tokens[2]};
    }
}

FilterfileParser::UnknownOriginKindError::UnknownOriginKindError(const std::string &loc_kind_text) {
    auto buf = std::stringstream{};
    buf << SyntaxError::what() << ": unknown location kind \"" << loc_kind_text
        << R"(" (it should be one of "src", "obj" or "fun"))";
    text = buf.str();
}

FilterfileParser::UnknownMessageIdError::UnknownMessageIdError(const std::string &msg_id_text) {
    auto buf = std::stringstream{};
    buf << ValueError::what() << ": unknown message type \"" << msg_id_text << "\"";
    text = buf.str();
}

} // namespace filter
} // namespace must
