#include "data/gpu_data.hpp"

FormattedGpuData format_gpu_data(const GpuInfo &gpu) {
  constexpr double bytes_per_mib = 1024.0 * 1024.0;

  FormattedGpuData formatted;
  formatted.name = gpu.name;
  formatted.memory_used_mib = gpu.memoryUsed / bytes_per_mib;
  formatted.memory_total_mib = gpu.memoryTotal / bytes_per_mib;
  formatted.memory_usage_percent = gpu.utilization;
  formatted.temperature_celsius = gpu.temperature;
  return formatted;
}