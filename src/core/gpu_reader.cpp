// for now only nvidia supported later includeing amd and stuff TODO

#include "gpu_reader.hpp"
#include <dlfcn.h>

void *nvml = nullptr;
bool init() {
  nvml = dlopen("libnvidia-ml.so", RTLD_LAZY);
  if (!nvml)
    return false;

  auto nvmlInit = (nvmlInit_t)dlsym(nvml, "nvmlInit_v2");
  if (!nvmlInit || nvmlInit() != 0) {
    dlclose(nvml);
    nvml = nullptr;
    return false;
  }

  return true;
}

GpuInfo gpu_info() {

  GpuInfo info;
  if (!nvml)
    return info;

  auto getHandle = (nvmlDeviceGetHandleByIndex_t)dlsym(
      nvml, "nvmlDeviceGetHandleByIndex_v2");

  auto getName = (nvmlDeviceGetName_t)dlsym(nvml, "nvmlDeviceGetName");
  if (!getHandle || !getName)
    return info;

  auto getMemoryInfo =
      (nvmlDeviceGetMemoryInfo_t)dlsym(nvml, "nvmlDeviceGetMemoryInfo");

  nvmlMemory_t memory{};

  nvmlDevice_t device = nullptr;

  if (getHandle(0, &device) != 0)
    return info;

  char name[100]{};
  if (getName(device, name, sizeof((name))) != 0)
    return info;

  info.name = name;
  if (getMemoryInfo && getMemoryInfo(device, &memory) == 0) {
    info.memoryUsed = memory.used;
    info.memoryTotal = memory.total;
  }
  if (info.memoryTotal > 0) {
    info.utilization =
        (static_cast<double>(info.memoryUsed) / info.memoryTotal) * 100.0;
  }
  auto getTemperature =
      (nvmlDeviceGetTemperature_t)dlsym(nvml, "nvmlDeviceGetTemperature");

  unsigned int temperature = 0;

  if (getTemperature && getTemperature(device, 0, &temperature) == 0) {
    info.temperature = temperature;
  }

  return info;
}