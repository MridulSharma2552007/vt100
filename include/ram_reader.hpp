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

  long pid = 0;
  long long size_in_kb = 0;
  std::string process_name;
};

struct ProcessDataBlockVector {
  std::vector<ProcessData> process_data;
};
ProcessDataBlockVector get_process_data();