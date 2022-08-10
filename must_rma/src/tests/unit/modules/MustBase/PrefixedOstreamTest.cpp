/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <ios>
#include <iostream>

#include <gtest/gtest.h>
#include <gtest/internal/gtest-port.h>
#include <sstream>

#include "PrefixedOstream.hpp"

TEST(PrefixedOstream, Multiline) { // NOLINT
  std::stringstream ss{};
  must::PrefixedOstream out{"PREFIX ", ss};

  out << "This should be prefixed" << std::endl
       << "And this too!" << std::endl;
  out << "This message has a \nlinebreak in the middle.";
  out.flush();
  EXPECT_EQ(ss.str(),
	    "PREFIX This should be prefixed\n"
	    "PREFIX And this too!\n"
	    "PREFIX This message has a \n"
	    "PREFIX linebreak in the middle.");
}

TEST(PrefixedOstream, MixedStreams) { // NOLINT
  /* "Don't cross the streams!" ;-) */
  std::stringstream ss{};
  must::PrefixedOstream prefixed{"OTHER PREFIX ", ss};

  prefixed << "This should be prefixed" << std::endl;
  ss << "This should not be prefixed" << std::endl;

  EXPECT_EQ(ss.str(),
	    "OTHER PREFIX This should be prefixed\n"
	    "This should not be prefixed\n");
}

TEST(PrefixedOstream, DefaultPrefixOstreams) { // NOLINT
  testing::internal::CaptureStdout();
  must::cout << "This should be prefixed with [MUST-RUNTIME]" << std::endl;
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output,
	    "[MUST-RUNTIME] This should be prefixed with [MUST-RUNTIME]\n");

  testing::internal::CaptureStderr();
  must::cerr << "This should be prefixed with [MUST-RUNTIME]" << std::endl;
  output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output,
	    "[MUST-RUNTIME] This should be prefixed with [MUST-RUNTIME]\n");

  testing::internal::CaptureStderr();
  must::clog << "This should be prefixed with [MUST-RUNTIME]" << std::endl;
  output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output,
	    "[MUST-RUNTIME] This should be prefixed with [MUST-RUNTIME]\n");
}

TEST(PrefixedOstream, FlushesOnDestruction) {
    std::stringstream ss{};
    auto const prefix = std::string{"OTHER PREFIX "};
    auto const msg = std::string{"Is this in ss after deconstruction without explicit flushing?"};
    // Extra scope to yield destruction
    {
        must::PrefixedOstream prefixed{prefix, ss};
        prefixed << msg;
    }
    EXPECT_EQ(ss.str(), prefix + msg);
}

TEST(PrefixedOstream, FlushesProperly) {
    std::stringstream ss{};
    auto const prefix = std::string{"OTHER PREFIX "};
    must::PrefixedOstream prefixed{prefix, ss};
    auto const msg = std::string{"Flush me!"};

    prefixed << msg;
    prefixed.flush();

    EXPECT_EQ(ss.str(), prefix + msg);
}
