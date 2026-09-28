

#include "cpu_reader.hpp"

#include <cctype>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>

cpuInfo read_cpu_info() {
  cpuInfo info;

  std::ifstream file("/proc/cpuinfo");
  std::string line;

  while (std::getline(file, line)) {

    // info's

    if (line.rfind("model name", 0) == 0) {

      auto pos = line.find(":");
      if (pos == std::string::npos)
        continue;
      std::string value = line.substr(pos + 1);
      info.model_name = value.substr(value.find_first_not_of(" \t"));
    }

    if (line.rfind("cpu MHz", 0) == 0) {

      auto pos = line.find(":");
      if (pos == std::string::npos)
        continue;

      std::string value = line.substr(pos + 1);
      std::string mhz_in_string = value.substr(value.find_first_not_of(" \t"));
      info.cpu_mhz = std::stod(mhz_in_string);
    }

    if (line.rfind("cpu cores", 0) == 0) {

      auto pos = line.find(":");
      if (pos == std::string::npos)
        continue;

      std::string value = line.substr(pos + 1);
      std::string mhz_in_string = value.substr(value.find_first_not_of(" \t"));
      info.cpu_cores = std::stoi(value);
    }

    if (line.rfind("cache size", 0) == 0) {

      auto pos = line.find(":");
      if (pos == std::string::npos)
        continue;

      std::string value = line.substr(pos + 1);

      std::string cache_in_string =
          value.substr(value.find_first_not_of(" \t"));

      info.cache_size = std::stod(cache_in_string);
    }
  }
  return info;
}

/// for dynamic data

AllCpuTimes get_cpu_data() {
  AllCpuTimes data;

  std::ifstream file("/proc/stat");
  std::string line;

  while (std::getline(file, line)) {
    if (line.rfind("cpu", 0) == 0) {

      // for overall because first line is ==>cpu  31856 151 8079 9707078 3482
      // 2595 2094 0 0 0
      if (line[3] == ' ') {
        std::istringstream iss(line);
        std::string label;
        iss >> label >> data.overall.user >> data.overall.nice >>
            data.overall.system >> data.overall.idle >> data.overall.iowait >>
            data.overall.irq >> data.overall.softirq >> data.overall.steal;
      }
      // for standalone cpus like cpu1 cpu2 etc...
      else if (isdigit(line[3])) {

        CpuTimes core;
        std::istringstream iss(line);
        std::string label;

        iss >> label >> core.user >> core.nice >> core.system >> core.idle >>
            core.iowait >> core.irq >> core.softirq >> core.steal;

        data.cores.push_back(core);
      }
    }
  }
  return data;
}

CpuMisc get_cpu_misc_data() {
  CpuMisc misc;

  std::ifstream file("/proc/loadavg");
  std::string line;

  while (std::getline(file, line)) {

    std::istringstream iss(line);
    iss >> misc.load.one_min >> misc.load.five_min >> misc.load.fifteen_min;
  }

  // temps
  int zone_index = 0;

  while (true) {

    std::string base_path =
        "/sys/class/thermal/thermal_zone" + std::to_string(zone_index);

    std::ifstream type_file(base_path + "/type");
    std::ifstream temp_file(base_path + "/temp");

    if (!type_file.is_open() || !temp_file.is_open()) {
      break;
    }

    std::string sensorName;
    std::string raw_temp;

    if (std::getline(type_file, sensorName)) {

      if (std::getline(temp_file, raw_temp)) {

        double maskedtemp = std::stod(raw_temp);

        temperature tempsStructure;

        tempsStructure.degrees = maskedtemp / 1000;
        tempsStructure.sensorName = sensorName;

        misc.temps.push_back(tempsStructure);
      }
    }

    zone_index++;
  }

  return misc;
}
