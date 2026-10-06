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
};

class GpuReader {
public:
  GpuReader();
  ~GpuReader();

  bool init();
  GpuInfo read();

private:
  void *nvml;

  nvmlInit_t nvmlInit;
  nvmlDeviceGetHandleByIndex_t getHandle;
  nvmlDeviceGetName_t getName;

  nvmlDevice_t device;
};