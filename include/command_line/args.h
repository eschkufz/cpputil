// Copyright 2014 eric schkufza
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CPPUTIL_INCLUDE_COMMAND_LINE_ARGS_H
#define CPPUTIL_INCLUDE_COMMAND_LINE_ARGS_H

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/command_line/arg.h"
#include "include/command_line/arg_registry.h"
#include "include/patterns/singleton.h"

namespace cpputil {

class Args {
 private:
  /** POD for recording argument parses */
  struct Parse {
    /** Args for which errors occurred */
    std::vector<Arg*> errors;
    /** Args which appeared more than once */
    std::vector<Arg*> duplicates;
    /** Args which were unrecognized (never registered) */
    std::vector<std::string> unrecognized;
    /** Values without associated arg names */
    std::vector<std::string> anonymous;
  };

 public:
  /** Iterator over registered args */
  typedef ArgRegistry::arg_iterator arg_iterator;

  /** First registered arg */
  static arg_iterator arg_begin() {
    return Singleton<ArgRegistry>::get().arg_begin();
  }

  /** Last registered arg */
  static arg_iterator arg_end() {
    return Singleton<ArgRegistry>::get().arg_end();
  }

  /** Iterator over registered arg groups */
  typedef ArgRegistry::group_iterator group_iterator;

  /** First registered group */
  static group_iterator group_begin() {
    return Singleton<ArgRegistry>::get().group_begin();
  }

  /** Last registered group */
  static group_iterator group_end() {
    return Singleton<ArgRegistry>::get().group_end();
  }

  /** Did any args signal errors? */
  static bool error() {
    return !Singleton<Parse>::get().errors.empty();
  }

  /** Iterator over args that signaled errors */
  typedef std::vector<Arg*>::iterator error_iterator;

  /** First arg that signaled error */
  static error_iterator error_begin() {
    return Singleton<Parse>::get().errors.begin();
  }

  /** Last arg that signaled error */
  static error_iterator error_end() {
    return Singleton<Parse>::get().errors.end();
  }

  /** Were any args duplicated? */
  static bool duplicate() {
    return !Singleton<Parse>::get().duplicates.empty();
  }

  /** Iterator over args with duplicate appearances */
  typedef std::vector<Arg*>::iterator duplicate_iterator;

  /** First duplicate arg */
  static duplicate_iterator duplicate_begin() {
    return Singleton<Parse>::get().duplicates.begin();
  }

  /** Last duplicate arg */
  static duplicate_iterator duplicate_end() {
    return Singleton<Parse>::get().duplicates.end();
  }

  /** Were any args unrecognized? */
  static bool unrecognized() {
    return !Singleton<Parse>::get().unrecognized.empty();
  }

  /** Iterator over unrecognized args */
  typedef std::vector<std::string>::const_iterator unrecognized_iterator;

  /** First unrecognized arg */
  static unrecognized_iterator unrecognized_begin() {
    return Singleton<Parse>::get().unrecognized.begin();
  }

  /** Last unrecognized arg */
  static unrecognized_iterator unrecognized_end() {
    return Singleton<Parse>::get().unrecognized.end();
  }

  /** Were any values provided without corresponding args? */
  static bool anonymous() {
    return !Singleton<Parse>::get().anonymous.empty();
  }

  /** Iterator over values without corresponding args */
  typedef std::vector<std::string>::const_iterator anonymous_iterator;

  /** First anonymous value */
  static anonymous_iterator anonymous_begin() {
    return Singleton<Parse>::get().anonymous.begin();
  }

  /** Last anonymous value */
  static anonymous_iterator anonymous_end() {
    return Singleton<Parse>::get().anonymous.end();
  }

  /** Sort arg groups */
  template <typename Comp>
  static void sort_groups(Comp c) {
    std::sort(group_begin(), group_end(), c);
  }

  /** Sort args within groups */
  template <typename Comp>
  static void sort_args(Comp c) {
    for (auto g = group_begin(); g != group_end(); ++g) {
      std::sort(g->arg_begin(), g->arg_end(), c);
    }
  }

  /** Read arguments in standard argc/argv format */
  static void read(int argc, char** argv) {
    auto& parse = Singleton<Parse>::get();
    parse.errors.clear();
    parse.duplicates.clear();
    parse.unrecognized.clear();
    parse.anonymous.clear();

    auto& arg_reg = Singleton<ArgRegistry>::get();
    std::vector<bool> used(argc);
    for (auto a = arg_reg.arg_begin(), ae = arg_reg.arg_end(); a != ae; ++a) {
      const auto res = (*a)->read(argc, argv);
      for (auto i = res.first; i <= res.second; ++i) {
        used[i] = true;
      }

      for (auto i = (*a)->appearance_begin(); i != (*a)->appearance_end(); ++i) {
        used[*i] = true;
      }
      if ((*a)->duplicated()) {
        parse.duplicates.push_back(*a);
      }

      if (!(*a)->good()) {
        parse.errors.push_back(*a);
      }
    }

    for (int i = 1; i < argc; ++i) {
      if (!used[i]) {
        if (argv[i][0] == '-') {
          parse.unrecognized.push_back(argv[i]);
        } else {
          parse.anonymous.push_back(argv[i]);
        }
      }
    }
  }

  /** Read arguments from an istream */
  static void read(std::istream& is) {
    std::stringstream contents;
    contents << is.rdbuf();

    std::ostringstream stripped;
    for (size_t i = 0, ie = contents.str().length(); i < ie; ++i)
      if (contents.str()[i] == '#') {
        while (i < ie && contents.str()[i++] != '\n');
        stripped << '\n';
        --i;
      } else {
        stripped << contents.str()[i];
      }

    std::vector<char*> argv;
    argv.push_back(strdup("<ignore>"));

    std::istringstream iss(stripped.str());
    std::string s;
    while (true) {
      iss >> s;
      if (iss.eof()) {
        break;
      }
      argv.push_back(strdup(s.c_str()));
    }

    read(argv.size(), argv.data());

    for (auto s : argv) {
      free(s);
    }
  }
};

} // namespace cpputil

#endif
