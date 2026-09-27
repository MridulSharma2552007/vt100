#include <iostream>

// <<<--header files->>>
#include "cpu_reader.hpp"
#include "ram_reader.hpp"

int main() {

  MemInfo info = read_mem_info();
  cpuInfo cpuinfo = read_cpu_info();
  AllCpuTimes cpuData = get_cpu_data();
  std::cout << "used Ram:" << info.total_kb / 1048576.0 << " Gb \n";
  std::cout << "used Ram:" << info.available_kb / 1048576.0 << " Gb \n";
  std::cout << "used Ram:" << info.used_kb / 1048576.0 << " Gb \n";
  std::cout << "cpu model:" << cpuinfo.model_name << "\n";
  std::cout << "cpu mhz:" << cpuinfo.cpu_mhz << "\n";
  std::cout << "cpu mhz:" << cpuinfo.cpu_cores << "\n";
  std::cout << "cpu cache:" << cpuinfo.cache_size / 1024 << "MB\n";

  std::cout << "\n--- Overall CPU times ---\n";
  std::cout << "user: " << cpuData.overall.user << "\n";
  std::cout << "nice: " << cpuData.overall.nice << "\n";
  std::cout << "system: " << cpuData.overall.system << "\n";
  std::cout << "idle: " << cpuData.overall.idle << "\n";
  std::cout << "iowait: " << cpuData.overall.iowait << "\n";

  std::cout << "\n--- Per-core count ---\n";
  std::cout << "cores found: " << cpuData.cores.size() << "\n";

  std::cout << "\n--- Per-core CPU times ---\n";
  for (size_t i = 0; i < cpuData.cores.size(); i++) {
    std::cout << "core " << i << " -> user: " << cpuData.cores[i].user
              << ", idle: " << cpuData.cores[i].idle << "\n";
  }

  return 0;
}
