#include <fstream>
#include <iostream>
#include <string>

// <<<--header files->>>
#include "cpu_reader.hpp"
#include "ram_reader.hpp"

int main() {

  MemInfo info = read_mem_info();
  cpuInfo cpuinfo = read_cpu_info();
  std::cout << "used Ram:" << info.total_kb / 1048576.0 << " Gb \n";
  std::cout << "used Ram:" << info.available_kb / 1048576.0 << " Gb \n";
  std::cout << "used Ram:" << info.used_kb / 1048576.0 << " Gb \n";
  std::cout << "cpu model:" << cpuinfo.model_name << "\n";
  std::cout << "cpu mhz:" << cpuinfo.cpu_mhz << "\n";
  std::cout << "cpu mhz:" << cpuinfo.cpu_cores << "\n";
  std::cout << "cpu cache:" << cpuinfo.cache_size / 1024 << "MB\n";

  return 0;
}
