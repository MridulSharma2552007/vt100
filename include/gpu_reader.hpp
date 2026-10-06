#pragma once

#include <string>

struct GpuInfo {
  std::string name;
  unsigned int utilization;
  unsigned long long memory_used;
  unsigned long long memory_total;
  unsigned int temperature;
  unsigned int power;
};

class GpuReader {
public:
  GpuReader();
  ~GpuReader();

  bool init();
  GpuInfo read();

private:
  void *nvml;
};