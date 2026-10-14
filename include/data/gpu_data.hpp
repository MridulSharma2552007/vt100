#pragma once

#include "gpu_reader.hpp"

#include <string>

struct FormattedGpuData {
  std::string name;
  double memory_used_mib = 0;
  double memory_total_mib = 0;
  double memory_usage_percent = 0;
  unsigned int temperature_celsius = 0;
};

FormattedGpuData format_gpu_data(const GpuInfo &gpu);