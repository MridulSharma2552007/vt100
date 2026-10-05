#pragma once
class GpuReader {
public:

  GpuReader();
  ~GpuReader();

  bool init();
  int getUtilization();
};