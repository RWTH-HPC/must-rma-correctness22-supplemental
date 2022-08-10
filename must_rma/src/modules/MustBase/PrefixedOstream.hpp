/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <ios>
#include <ostream>
#include <string>
#include <iostream>
#include <utility>
#include <sstream>

#include "MustDefines.h"

namespace must {
  /**
   * Prefix all lines of an std::ostream.
   *
   * This class is to be used as a drop-in replacement for the default
   * console output streams of the standard library.
   *
   * Example:
   * ```cpp
   * PrefixedOstream pcout{"[PREFIX] ", std::cout};
   * pcout << "Foo\nBar" << std::endl;
   * std::cout << "Not prefixed" << std::endl;
   * ```
   * yields
   * ```
   * [PREFIX] Foo
   * [PREFIX] Bar
   * Not prefixed
   * ```
   */
  class PrefixedOstream : std::stringbuf, public std::ostream {
  public:
    /**
     * @param prefix The string put in front of every line
     * @param recv_stream The stream to send the prefixed lines to. It must
              outlive the constructed PrefixOstream.
     */
    PrefixedOstream(std::string prefix, std::ostream &recv_stream)
      : std::stringbuf{ios_base::out},
      std::ostream(this),
      prefix(std::move(prefix)),
      receiving_stream(recv_stream) {};

    ~PrefixedOstream() override { this->flush(); }

  private:
    /** The prefix to be put in front of every line. */
    const std::string prefix;
    /** The wrapped stream. */
    std::ostream& receiving_stream;
    /** True if the prefix should be emitted on the next char. */
    bool fresh_line = true;

    auto format(const std::string& s)  -> std::string {
      if (s.empty()) {
	return "";
      }
      using strsize_t = std::string::size_type;
      std::string res;
      res.reserve(s.length()+prefix.length());
      if (fresh_line) {
	res.append(prefix);
      }
      for(strsize_t i = 0; i < s.length()-1; ++i) {
	char c = s[i];
	res.push_back(c);
	if (c == '\n') {
	  res.append(prefix);
	}
      }
      res.push_back(s.back());
      return res;
    }

    auto sync() -> int override {
      if (!str().empty()) {
	std::string prefixed = format(str());
	receiving_stream.write(prefixed.data(), static_cast<std::streamsize>(prefixed.length()));
	receiving_stream.flush();
	if (prefixed.back() == '\n') {
	  fresh_line = true;
	}
	str("");
      }
      return 0;
    }
  };

  // It should be safe to use std::cout, etc. here, because
  // std::ios_base::Init is already included through the iostream
  // header. This ensures that they are already initialized.
  /** Prefixing drop-in replacement for std::cout */
  static PrefixedOstream cout{MUST_STDOUT_PREFIX_RUNTIME, std::cout};
  /** Prefixing drop-in replacement for std::cerr */
  static PrefixedOstream cerr{MUST_STDOUT_PREFIX_RUNTIME, std::cerr};
  /** Prefixing drop-in replacement for std::clog */
  static PrefixedOstream clog{MUST_STDOUT_PREFIX_RUNTIME, std::clog};
}
