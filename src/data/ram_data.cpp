#include "data/ram_data.hpp"

FormattedRamData format_ram_data(const MemInfo &memory,
                                 const ProcessDataBlockVector &processes) {
  constexpr double kib_per_gib = 1024.0 * 1024.0;
  constexpr double kib_per_mib = 1024.0;

  FormattedRamData formatted;
  formatted.total_gib = memory.total_kb / kib_per_gib;
  formatted.available_gib = memory.available_kb / kib_per_gib;
  formatted.used_gib = memory.used_kb / kib_per_gib;

  formatted.processes.reserve(processes.process_data.size());
  for (const ProcessData &process : processes.process_data) {
    formatted.processes.push_back(
        {process.pid, process.size_in_kb / kib_per_mib, process.process_name});
  }

  return formatted;
}