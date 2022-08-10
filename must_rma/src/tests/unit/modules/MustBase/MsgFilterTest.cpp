/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MsgFilter.hpp"

#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

using namespace must::filter;

auto create_istream(const char *input) -> std::istringstream { return std::istringstream{input}; }

using Stacktrace = std::vector<MustStackLevelInfo>;
auto make_stacklevel(std::string symName, std::string fileModule) -> MustStackLevelInfo {
    auto stacklevel = MustStackLevelInfo{};
    stacklevel.symName = std::move(symName);
    stacklevel.fileModule = std::move(fileModule);
    return stacklevel;
};

class MsgInfoImpl : public MsgInfo<std::vector<MustStackLevelInfo>::const_iterator> {
  public:
    using Callstack = std::vector<MustStackLevelInfo>;
    must::MustMessageIdNames id;
    Callstack stack;

    MsgInfoImpl(must::MustMessageIdNames id, std::vector<MustStackLevelInfo> stack)
        : id{id}, stack{std::move(stack)} {};

    auto msg_id() const -> must::MustMessageIdNames override { return id; }
    auto stack_begin() const -> Callstack::const_iterator override { return stack.cbegin(); }
    auto stack_end() const -> Callstack::const_iterator override { return stack.cend(); }
};

TEST(MsgFilter, ParserReadsFromAnyIstream) {
    const std::string input = "# Some example file\n"
                              "messageType: MUST_ERROR_MESSAGE_LOST:fun:somefunc\n";
    // Force pointer to base class
    auto input_stream = std::unique_ptr<std::istream>{new std::istringstream{input}};

    auto parser = FilterfileParser{};
    parser.parse(*input_stream);
}

TEST(MsgFilter, ParserProvidesMessageTypeRules) {
    const auto *const input = "# Some example file\n"
                              "messageType: MUST_ERROR_MESSAGE_LOST:fun:somefunc\n"
                              "messageType: MUST_ERROR_MESSAGE_LOST:src:somefile.c\n";
    auto stream = create_istream(input);

    auto parser = FilterfileParser{};
    const bool success = parser.parse(stream);
    EXPECT_TRUE(success);
}

TEST(MsgFilter, ParserHandlesErroneousLinesWithoutStopping) {
    const auto *const input = "# Some example file\n"
                              "this \n"
                              "messageType: MUST_ERROR_MESSAGE_LOST:fun:somefunc\n";
    auto input_stream = create_istream(input);

    auto parser = FilterfileParser{};
    bool success = parser.parse(input_stream);
    EXPECT_FALSE(success);

    EXPECT_FALSE(parser.errors().empty());
}

TEST(MsgFilter, ParserIgnoresComments) {
    const auto *const input = "# Some example file\n"
                              "#messageType: MUST_ERROR_DATATYPE_NULL:src:src.c\n"
                              "messageType: MUST_ERROR_MESSAGE_LOST:fun:somefunc\n"
                              "#\n";
    auto input_stream = create_istream(input);

    auto parser = FilterfileParser{};
    bool success = parser.parse(input_stream);
    EXPECT_TRUE(success);
}

TEST(MsgFilter, ParserReportsCorrectLinenumberOnError) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"\nsome false line\n"};
    auto success = parser.parse(in);
    EXPECT_FALSE(success);

    EXPECT_EQ(parser.errors().size(), 1);
    const auto &err = parser.errors().at(0);
    EXPECT_EQ(err.lineno(), 2);
}

TEST(MsgFilter, ParserReportsFilterKindError) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"some false line\n"};
    auto success = parser.parse(in);
    EXPECT_FALSE(success);

    EXPECT_EQ(parser.errors().size(), 1);
    const auto &err = parser.errors().at(0);
    EXPECT_EQ(err.lineno(), 1);
    EXPECT_EQ(err.content(), "some false line");
    EXPECT_EQ(err.msg(), "unknown filter kind: \"some false line\"");
}

TEST(MsgFilter, ParserReportsSyntaxError) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"messageType:MUST_ERROR_DATATYPE_NULL\n"};
    parser.parse(in);

    const auto &err = parser.errors().at(0);
    EXPECT_EQ(err.content(), "messageType:MUST_ERROR_DATATYPE_NULL");
    EXPECT_EQ(err.msg(), "syntax error");
}

TEST(MsgFilter, ParserReportsUnknownMessageId) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"messageType:SOME_FALSE_MESSAGE_ID:fun:*\n"};
    parser.parse(in);

    const auto &err = parser.errors().at(0);
    EXPECT_EQ(err.content(), "messageType:SOME_FALSE_MESSAGE_ID:fun:*");
    EXPECT_EQ(err.msg(), "value error: unknown message type \"SOME_FALSE_MESSAGE_ID\"");
}

TEST(MsgFilter, ParserReportsUnknownOriginKind) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"messageType:MUST_ERROR_DATATYPE_NULL:loc:*\n"};

    parser.parse(in);

    const auto &err = parser.errors().at(0);
    EXPECT_EQ(err.msg(), "syntax error: unknown location kind \"loc\" (it should be one of "
                         "\"src\", \"obj\" or \"fun\")");
}

TEST(MsgFilter, FilterMatchesAgainstListOfRules) {
    auto parser = FilterfileParser();
    auto in = std::istringstream{"messageType: MUST_ERROR_DATATYPE_NULL:src:src.c\n"
                                 "messageType: MUST_ERROR_DATATYPE_NULL:fun:some_func\n"
                                 "messageType: MUST_ERROR_DATATYPE_NULL:object:something.o\n"};
    parser.parse(in);
    const auto filter = MsgFilter<MsgInfoImpl::Callstack::const_iterator>{parser};

    {
        auto msg_info = MsgInfoImpl{must::MUST_ERROR_DATATYPE_NULL,
                                    Stacktrace{
                                        make_stacklevel("func1", "unknown_source.c"),
                                        make_stacklevel("some_func", "unknown_source.c"),
                                    }};
        EXPECT_TRUE(filter.match(msg_info));
    }
    {
        auto msg_info = MsgInfoImpl{must::MUST_ERROR_DATATYPE_NULL,
                                    Stacktrace{
                                        make_stacklevel("func1", "unknown_source.c"),
                                        make_stacklevel("func2", "unknown_source.c"),
                                    }};
        EXPECT_FALSE(filter.match(msg_info));
    }
}

TEST(MsgFilter, FiltersWildcardOrigins) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"messageType: MUST_ERROR_MESSAGE_LOST:*\n"
                                 "messageType: MUST_ERROR_DATATYPE_NULL:fun:some_func\n"};
    parser.parse(in);

    const auto filter = MsgFilter<MsgInfoImpl::Callstack::const_iterator>{parser};
    {
        auto info = MsgInfoImpl{
            must::MUST_ERROR_MESSAGE_LOST,
            std::vector<MustStackLevelInfo>{},
        };
        EXPECT_TRUE(filter.match(info));
    }
    {
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            std::vector<MustStackLevelInfo>{},
        };
        EXPECT_FALSE(filter.match(info));
    }
}

TEST(MsgFilter, FiltersWildcardOriginOptimization) {
    auto parser = FilterfileParser{};
    // We know that the filter applies some optimization internally when
    // encountering a wildcard spec that filters the same type as a previous rule.
    auto in = std::istringstream{"messageType: MUST_ERROR_MESSAGE_LOST:fun:some_func\n"
                                 "messageType: MUST_ERROR_MESSAGE_LOST:*\n"};
    parser.parse(in);

    const auto filter = MsgFilter<MsgInfoImpl::Callstack::const_iterator>{parser};
    {
        auto info = MsgInfoImpl{
            must::MUST_ERROR_MESSAGE_LOST,
            std::vector<MustStackLevelInfo>{},
        };
        EXPECT_TRUE(filter.match(info));
    }
}

TEST(MsgFilter, FiltersSymbols) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"messageType: MUST_ERROR_DATATYPE_NULL:fun:some_func\n"};
    parser.parse(in);

    const auto filter = MsgFilter<MsgInfoImpl::Callstack::const_iterator>{parser};

    { // simple match
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "unknown.c")},
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // function to match is at the bottom of the stacktrace
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{
                make_stacklevel("irrelevant_func", "unknown.c"),
                make_stacklevel("some_func", "unknown.c"),
            },
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // function to match is at the top of the stacktrace
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{
                make_stacklevel("some_func", "unknown.c"),
                make_stacklevel("irrelevant_func", "unknown.c"),
            },
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // MsgId matches but empty stacktrace is given
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{},
        };
        EXPECT_FALSE(filter.match(info));
    }
    { // stacktrace full of similar but slightly different symbols
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{
                make_stacklevel("ome_func", "somewhere.o"),
                make_stacklevel("ssome_func", "somewhere.o"),
                make_stacklevel("some_fun", "somewhere.o"),
                make_stacklevel("some_funcc", "somewhere.o"),
            },
        };
        EXPECT_FALSE(filter.match(info));
    }
    { // the function matches but not the MessageTypeId
        auto info = MsgInfoImpl{
            must::MUST_ERROR_MESSAGE_LOST,
            Stacktrace{make_stacklevel("some_func", "unknown.c")},
        };
        EXPECT_FALSE(filter.match(info));
    }
}

TEST(MsgFilter, FiltersOrigins) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"messageType: MUST_ERROR_DATATYPE_NULL:src:main.c\n"
                                 "messageType: MUST_ERROR_DATATYPE_NULL:obj:code.o\n"};
    parser.parse(in);

    const auto filter = MsgFilter<MsgInfoImpl::Callstack::const_iterator>{parser};

    { // simple match
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "main.c")},
        };
        EXPECT_TRUE(filter.match(info));
        info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "code.o")},
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // match files in paths
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "fancy_proj/src/main.c")},
        };
        EXPECT_TRUE(filter.match(info));
        info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "/some/dir/code.o")},
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // origin to match is at the bottom of the stacktrace
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{
                make_stacklevel("irrelevant_func", "unknown.c"),
                make_stacklevel("some_func", "main.c"),
            },
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // source file to match is at the top of the stacktrace
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{
                make_stacklevel("some_func", "main.c"),
                make_stacklevel("irrelevant_func", "unknown.c"),
            },
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // MsgId matches but empty stacktrace is given
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{},
        };
        EXPECT_FALSE(filter.match(info));
    }
    { // stacktrace full of similar but slightly different names
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{
                make_stacklevel("some_func", "main.cc"),
                make_stacklevel("some_func", "mmain.c"),
                make_stacklevel("some_func", "ain.c"),
                make_stacklevel("some_func", "main."),
            },
        };
        EXPECT_FALSE(filter.match(info));
    }
    { // the name matches but not the MessageTypeId
        auto info = MsgInfoImpl{
            must::MUST_ERROR_MESSAGE_LOST,
            Stacktrace{make_stacklevel("some_func", "main.c")},
        };
        EXPECT_FALSE(filter.match(info));
    }
}

TEST(MsgFilter, FiltersPartialPaths) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"messageType: MUST_ERROR_DATATYPE_NULL:src:project/main.c\n"};
    parser.parse(in);

    const auto filter = MsgFilter<MsgInfoImpl::Callstack::const_iterator>{parser};

    { // simple match
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "project/main.c")},
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // simple match
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "other/main.c")},
        };
        EXPECT_FALSE(filter.match(info));
    }
    { // simple match
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "other_project/main.c")},
        };
        EXPECT_FALSE(filter.match(info));
    }
}

TEST(MsgFilter, FiltersAbsolutePaths) {
    auto parser = FilterfileParser{};
    auto in = std::istringstream{"messageType: MUST_ERROR_DATATYPE_NULL:src:project/main.c\n"};
    parser.parse(in);

    const auto filter = MsgFilter<MsgInfoImpl::Callstack::const_iterator>{parser};

    { // simple match
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "/code/project/main.c")},
        };
        EXPECT_TRUE(filter.match(info));
    }
    { // simple match
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "/code/other/main.c")},
        };
        EXPECT_FALSE(filter.match(info));
    }
    { // simple match
        auto info = MsgInfoImpl{
            must::MUST_ERROR_DATATYPE_NULL,
            Stacktrace{make_stacklevel("some_func", "/code/other_project/main.c")},
        };
        EXPECT_FALSE(filter.match(info));
    }
}
