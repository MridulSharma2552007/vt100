#pragma once

#include <string>
#include <vector>



//<<<<<<<<-------Static data---------------------->>>>>>>>



struct cpuInfo {
  double cpu_mhz = 0;
  int cpu_cores = 0;
  long cache_size = 0;
  std::string model_name;
};

cpuInfo read_cpu_info();




//<<<---------For dynamic data---------------->>>>

// CPU Data
struct CpuTimes {
  long long user = 0;
  long long nice = 0;
  long long system = 0;
  long long idle = 0;
  long long iowait = 0;
  long long irq = 0;
  long long softirq = 0;
  long long steal = 0;
};

struct AllCpuTimes {
  CpuTimes overall; // cpu times struct as input field for cores vector
  std::vector<CpuTimes> cores;
};
AllCpuTimes get_cpu_data();