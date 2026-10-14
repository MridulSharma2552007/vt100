#pragma once

#include "ram_reader.hpp"

#include <string>
#include <vector>

struct ProcessMemoryData {
  long pid = 0;
  double memory_mib = 0;
  std::string process_name;
};

struct FormattedRamData {
  double total_gib = 0;
  double available_gib = 0;
  double used_gib = 0;
  std::vector<ProcessMemoryData> processes;
};

FormattedRamData format_ram_data(const MemInfo &memory,
                                 const ProcessDataBlockVector &processes);