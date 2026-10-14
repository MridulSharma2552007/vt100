#include "data/cpu_data.hpp"

FormattedCpuData format_cpu_data(const cpuInfo &info, const AllCpuTimes &times,
                                 const CpuMisc &misc) {
  FormattedCpuData formatted;
  formatted.model_name = info.model_name;
  formatted.clock_ghz = info.cpu_mhz / 1000.0;
  formatted.cores = info.cpu_cores;
  formatted.cache_mib = info.cache_size / 1024.0;
  formatted.raw_times = times;
  formatted.load = misc.load;

  formatted.temperatures.reserve(misc.temps.size());
  for (const temperature &sensor : misc.temps) {
    formatted.temperatures.push_back({sensor.sensorName, sensor.degrees});
  }

  formatted.frequencies.reserve(misc.frequencies.size());
  for (const cpufreq &frequency : misc.frequencies) {
    formatted.frequencies.push_back(
        {frequency.cpu, frequency.frequency / 1000.0});
  }

  return formatted;
}