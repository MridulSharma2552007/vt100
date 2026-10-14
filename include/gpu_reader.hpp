#pragma once

#include <string>

using nvmlReturn_t = int;
using nvmlDevice_t = void *;

using nvmlInit_t = nvmlReturn_t (*)();

using nvmlDeviceGetHandleByIndex_t = nvmlReturn_t (*)(unsigned int,
                                                      nvmlDevice_t *);

using nvmlDeviceGetName_t = nvmlReturn_t (*)(nvmlDevice_t, char *,
                                             unsigned int);

struct GpuInfo {
  std::string name;
  unsigned int utilization = 0;
  unsigned long long memoryUsed = 0;
  unsigned long long memoryTotal = 0;
  unsigned int temperature;
};

struct nvmlMemory_t {
  unsigned long long total;
  unsigned long long free;
  unsigned long long used;
};

using nvmlDeviceGetMemoryInfo_t = nvmlReturn_t (*)(nvmlDevice_t,
                                                   nvmlMemory_t *);
using nvmlDeviceGetTemperature_t = nvmlReturn_t (*)(nvmlDevice_t, unsigned int,
                                                    unsigned int *);

bool init();

GpuInfo gpu_info();