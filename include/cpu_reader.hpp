#pragma once

#include <string>

struct cpuInfo{
    long cpu_mhz=0;
    int cpu_cores=0;
    long cache_size=0;
    std::string model_name;
};
cpuInfo read_cpu_info();