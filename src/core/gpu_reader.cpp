// for now only nvidia supported later includeing amd and stuff TODO

#include "gpu_reader.hpp"

#include <dlfcn.h> //dynamic linking

GpuReader::GpuReader() {}

GpuReader::~GpuReader() {}

bool GpuReader::init() {
  void *nvml = dlopen("libnvidia-ml.so", RTLD_LAZY);
  if (!nvml) {
    return false;
  }
  return true;
}