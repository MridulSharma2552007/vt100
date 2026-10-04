#pragma once

#include <string>
#include <vector>
struct MemInfo {
  long total_kb = 0;
  long available_kb = 0;
  long used_kb = 0;
};

MemInfo read_mem_info();

struct ProcessData {
  // for pid and ram stuff

  long pid;
  long long size_in_kb;
  std::string process_name;
};

struct ProcessDataBlockVector {
  std::vector<ProcessData> process_data;
};
ProcessDataBlockVector get_process_data();