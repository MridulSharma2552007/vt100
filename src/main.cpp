#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

// <<<--header files->>>
#include "cpu_reader.hpp"
#include "ram_reader.hpp"

int main() {
  using namespace ftxui;
  MemInfo info = read_mem_info();
  CpuMisc misc = get_cpu_misc_data();
  Element document = hbox({
      text(std::to_string(info.available_kb)) | border,
      text("middle") | border | flex,
      text("right") | border,
  });

  auto screen = Screen::Create(Dimension::Full(),       // Width
                               Dimension::Fit(document) // Height

  );
  Render(screen, document);

  screen.Print();
  // todo:make a data layer to simplify data , then make graphs using that data

  cpuInfo cpuinfo = read_cpu_info();
  AllCpuTimes cpuData = get_cpu_data();
  std::cout << "used Ram:" << info.total_kb / 1048576.0 << " Gb \n";
  std::cout << "used Ram:" << info.available_kb / 1048576.0 << " Gb \n";
  std::cout << "used Ram:" << info.used_kb / 1048576.0 << " Gb \n";
  std::cout << "cpu model:" << cpuinfo.model_name << "\n";
  std::cout << "cpu mhz:" << cpuinfo.cpu_mhz << "\n";
  std::cout << "cpu mhz:" << cpuinfo.cpu_cores << "\n";
  std::cout << "cpu cache:" << cpuinfo.cache_size / 1024 << "MB\n";

  std::cout << "\n--- Overall CPU times ---\n";
  std::cout << "user: " << cpuData.overall.user << "\n";
  std::cout << "nice: " << cpuData.overall.nice << "\n";
  std::cout << "system: " << cpuData.overall.system << "\n";
  std::cout << "idle: " << cpuData.overall.idle << "\n";
  std::cout << "iowait: " << cpuData.overall.iowait << "\n";

  std::cout << "\n--- Per-core count ---\n";
  std::cout << "cores found: " << cpuData.cores.size() << "\n";

  std::cout << "\n--- Per-core CPU times ---\n";
  for (size_t i = 0; i < cpuData.cores.size(); i++) {
    std::cout << "core " << i << " -> user: " << cpuData.cores[i].user
              << ", idle: " << cpuData.cores[i].idle << "\n";
  }

  std::cout << "\t" << "1min  " << misc.load.one_min << "\n";
  std::cout << "\t" << "5min  " << misc.load.five_min << "\n";
  std::cout << "\t" << "15min "  << misc.load.fifteen_min << "\n";
  return 0;
}
