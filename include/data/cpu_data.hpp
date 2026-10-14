#pragma once

#include "cpu_reader.hpp"

#include <string>
#include <vector>

struct CpuTemperatureData {
  std::string sensor_name;
  double celsius = 0;
};

struct CpuFrequencyData {
  int cpu = 0;
  double mhz = 0;
};

struct FormattedCpuData {
  std::string model_name;
  double clock_ghz = 0;
  int cores = 0;
  double cache_mib = 0;
  AllCpuTimes raw_times;
  LoadAvg load;
  std::vector<CpuTemperatureData> temperatures;
  std::vector<CpuFrequencyData> frequencies;
};

FormattedCpuData format_cpu_data(const cpuInfo &info, const AllCpuTimes &times,
                                 const CpuMisc &misc);