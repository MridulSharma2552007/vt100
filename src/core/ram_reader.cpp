#include "ram_reader.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

MemInfo read_mem_info() {
  MemInfo info;

  std::ifstream file("/proc/meminfo");
  std::string line;

  while (std::getline(file, line)) {

    if (line.rfind("MemTotal:", 0) == 0) {
      std::istringstream iss(line);
      std::string label, unit;
      long value;

      iss >> label >> value >> unit;

      info.total_kb = value;
    } else if (line.rfind("MemAvailable:", 0) == 0) {
      std::istringstream iss(line);
      std::string unit, label;
      long value;

      iss >> label >> value >> unit;

      info.available_kb = value;
    }
  }
  info.used_kb = info.total_kb - info.available_kb;

  return info;
}

// for ram data over every pid
ProcessDataBlockVector get_process_data() {
  ProcessDataBlockVector process;

  for (const auto &entry : fs::directory_iterator("/proc")) {

    std::string name = entry.path().filename().string();

    bool is_pid =
        !name.empty() && std::all_of(name.begin(), name.end(), ::isdigit);

    if (is_pid) {
      int pid = std::stoi(name);

      std::string path("/proc/" + std::to_string(pid) + "/status");

      std::ifstream file(path);

      std::string line;
      ProcessData data;
      data.pid = pid;

      while (std::getline(file, line)) {

        if (line.rfind("VmRSS", 0) == 0) {

          auto pos = line.find(":");
          if (pos == std::string::npos)
            continue;
          data.size_in_kb = std::stoi(line.substr(pos + 1));
        }
        if (line.rfind("Name:", 0) == 0) {
          auto pos = line.find(":");
          if (pos == std::string::npos)
            continue;
          data.process_name = line.substr(pos + 1);
        }
      }
      process.process_data.push_back(data);
    }
  }

  return process;
};
