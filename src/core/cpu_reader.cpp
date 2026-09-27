

#include "cpu_reader.hpp"

#include <fstream>
#include <string>

cpuInfo read_cpu_info() {
  cpuInfo info;

  std::ifstream file("/proc/cpuinfo");
  std::string line;

  while (std::getline(file, line)) {

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
        std::string mhz_in_string =
            value.substr(value.find_first_not_of(" \t"));
        info.cpu_mhz = std::stod(mhz_in_string);
      }
    


      if (line.rfind("cpu cores", 0) == 0) {

        auto pos = line.find(":");
        if (pos == std::string::npos)
          continue;

        std::string value = line.substr(pos + 1);
        std::string mhz_in_string =
            value.substr(value.find_first_not_of(" \t"));
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
