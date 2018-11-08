/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "cyber/mainboard/module_argument.h"

#include <getopt.h>

using apollo::cyber::common::GlobalData;

namespace apollo {
namespace cyber {
namespace mainboard {

ModuleArgument::ModuleArgument() {}

ModuleArgument::~ModuleArgument() {}

void ModuleArgument::DisplayUsage() {
  AINFO << "Usage: \n    " << binary_name_ << " [OPTION]...\n"
        << "Description: \n"
        << "    -h, --help : help infomation \n"
        << "    -d, --dag_conf=CONFIG_FILE : module dag config file\n"
        << "    -p, --process_name=process_name: the process "
           "namespace for running this module, default in manager process\n"
        << "    -s, --sched_name=sched_name: sched policy "
           "conf for hole process, sched_name should be conf in cyber.pb.conf\n"
        << "Example:\n"
        << "    " << binary_name_ << " -h\n"
        << "    " << binary_name_
        << " -d dag_conf_file1 -d dag_conf_file2 "
        << "-p process_name -s sched_name\n";
}

bool ModuleArgument::ParseArgument(const int argc, char* const argv[]) {
  const std::string binary_name(argv[0]);
  binary_name_ = binary_name.substr(binary_name.find_last_of("/") + 1);
  GetOptions(argc, argv);

  if (process_name_.empty()) {
    process_name_ = DEFAULT_process_name_;
  }

  if (sched_name_.empty()) {
    sched_name_ = DEFAULT_sched_name_;
  }

  GlobalData::Instance()->SetProcessName(process_name_);
  GlobalData::Instance()->SetSchedName(sched_name_);
  AINFO << "binary_name_ is " << binary_name_ << ", process_name_ is "
        << process_name_ << ", has " << dag_conf_list_.size() << " dag conf";
  for (std::string& dag : dag_conf_list_) {
    AINFO << "dag_conf: " << dag;
  }
  return true;
}

void ModuleArgument::GetOptions(const int argc, char* const argv[]) {
  opterr = 0;  // extern int opterr
  int long_index = 0;
  const std::string short_opts = "h::d:p:s:";
  static const struct option long_opts[] = {
      {"help", no_argument, nullptr, 'h'},
      {"dag_conf", required_argument, nullptr, 'd'},
      {"process_name", required_argument, nullptr, 'p'},
      {"sched_name", required_argument, nullptr, 's'},
      {NULL, no_argument, nullptr, 0}};

  // log command for info
  std::string cmd("");
  for (int i = 0; i < argc; ++i) {
    cmd += argv[i];
    cmd += " ";
  }
  AINFO << "command: " << cmd;

  do {
    int opt =
        getopt_long(argc, argv, short_opts.c_str(), long_opts, &long_index);
    if (opt == -1) {
      break;
    }
    switch (opt) {
      case 'd':
        optind--;
        while (optind < argc) {
          if (*argv[optind] != '-') {
            dag_conf_list_.emplace_back(argv[optind]);
            optind++;
          } else {
            optind--;
            break;
          }
        }
        break;
      case 'p':
        process_name_ = std::string(optarg);
        break;
      case 's':
        sched_name_ = std::string(optarg);
        break;
      case 'h':
        DisplayUsage();
        exit(0);
      default:
        break;
    }
  } while (true);
}

}  // namespace mainboard
}  // namespace cyber
}  // namespace apollo
