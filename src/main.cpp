#include <algorithm>
#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "cpu_reader.hpp"
#include "data/cpu_data.hpp"
#include "data/gpu_data.hpp"
#include "data/ram_data.hpp"
#include "gpu_reader.hpp"
#include "ram_reader.hpp"

using namespace ftxui;

namespace {

struct Snapshot {
  FormattedCpuData cpu;
  FormattedGpuData gpu;
  FormattedRamData ram;
  AllCpuTimes raw_cpu;
  std::vector<double> core_usage;
  double cpu_usage = 0;
  std::string updated_at;
};

long long total_ticks(const CpuTimes &times) {
  return times.user + times.nice + times.system + times.idle + times.iowait +
         times.irq + times.softirq + times.steal;
}

double usage_between(const CpuTimes &before, const CpuTimes &after) {
  const long long total_delta = total_ticks(after) - total_ticks(before);
  const long long idle_delta =
      (after.idle + after.iowait) - (before.idle + before.iowait);
  if (total_delta <= 0)
    return 0;
  return std::clamp(100.0 * (total_delta - idle_delta) / total_delta, 0.0,
                    100.0);
}

std::string number(double value, int precision = 1) {
  std::ostringstream output;
  output << std::fixed << std::setprecision(precision) << value;
  return output.str();
}

std::string clock_text() {
  const std::time_t now = std::time(nullptr);
  std::tm local{};
  localtime_r(&now, &local);
  std::ostringstream output;
  output << std::put_time(&local, "%H:%M:%S");
  return output.str();
}

void refresh(Snapshot &snapshot, const AllCpuTimes &previous_cpu,
             bool has_previous) {
  const cpuInfo cpu_info = read_cpu_info();
  snapshot.raw_cpu = get_cpu_data();
  const CpuMisc misc = get_cpu_misc_data();
  snapshot.cpu = format_cpu_data(cpu_info, snapshot.raw_cpu, misc);
  snapshot.gpu = format_gpu_data(gpu_info());
  snapshot.ram = format_ram_data(read_mem_info(), get_process_data());
  snapshot.updated_at = clock_text();

  snapshot.core_usage.assign(snapshot.raw_cpu.cores.size(), 0);
  if (has_previous) {
    snapshot.cpu_usage =
        usage_between(previous_cpu.overall, snapshot.raw_cpu.overall);
    const size_t common_cores =
        std::min(previous_cpu.cores.size(), snapshot.raw_cpu.cores.size());
    for (size_t index = 0; index < common_cores; ++index) {
      snapshot.core_usage[index] = usage_between(previous_cpu.cores[index],
                                                 snapshot.raw_cpu.cores[index]);
    }
  }
}

Element labeled_value(const std::string &label, const std::string &value,
                      Color value_color = Color::RGB(218, 226, 237)) {
  return hbox({text(label) | color(Color::RGB(126, 148, 166)), filler(),
               text(value) | bold | color(value_color)});
}

Element metric_bar(const std::string &label, double percent, Color tint) {
  const float progress =
      static_cast<float>(std::clamp(percent, 0.0, 100.0) / 100.0);
  return vbox({hbox({text(label) | color(Color::RGB(126, 148, 166)), filler(),
                     text(number(percent) + "%") | bold | color(tint)}),
               gauge(progress) | color(tint)});
}

Element panel(const std::string &title, Elements content, Color accent) {
  return window(text(" " + title + " ") | bold | color(accent),
                vbox(std::move(content)) | xflex, ROUNDED) |
         borderStyled(accent) | xflex;
}

Element overview(const Snapshot &snapshot) {
  const Color mint = Color::RGB(94, 218, 177);
  const Color coral = Color::RGB(255, 154, 129);
  const Color sky = Color::RGB(111, 190, 245);
  const Color gold = Color::RGB(244, 202, 112);

  Elements cpu_lines = {
      labeled_value("Model", snapshot.cpu.model_name),
      labeled_value("Current clock", number(snapshot.cpu.clock_ghz, 2) + " GHz",
                    sky),
      labeled_value("Cores / cache",
                    std::to_string(snapshot.cpu.cores) + " / " +
                        number(snapshot.cpu.cache_mib, 0) + " MiB"),
      metric_bar("Total load", snapshot.cpu_usage, mint),
      separator(),
      labeled_value("Load average",
                    number(snapshot.cpu.load.one_min, 2) + "  " +
                        number(snapshot.cpu.load.five_min, 2) + "  " +
                        number(snapshot.cpu.load.fifteen_min, 2))};

  Elements gpu_lines = {
      labeled_value("Device", snapshot.gpu.name),
      labeled_value("VRAM",
                    number(snapshot.gpu.memory_used_mib) + " / " +
                        number(snapshot.gpu.memory_total_mib, 0) + " MiB",
                    sky),
      metric_bar("VRAM used", snapshot.gpu.memory_usage_percent, coral),
      separator(),
      labeled_value("Temperature",
                    std::to_string(snapshot.gpu.temperature_celsius) + " C",
                    gold)};

  Elements ram_lines = {
      labeled_value("Used", number(snapshot.ram.used_gib) + " GiB", coral),
      metric_bar("Memory pressure",
                 snapshot.ram.total_gib > 0
                     ? 100.0 * snapshot.ram.used_gib / snapshot.ram.total_gib
                     : 0,
                 mint),
      separator(),
      labeled_value("Available", number(snapshot.ram.available_gib) + " GiB",
                    sky),
      labeled_value("Total", number(snapshot.ram.total_gib) + " GiB")};

  return vbox(
      {hbox({panel("CPU / little workhorse", std::move(cpu_lines), mint),
             panel("GPU / pixel furnace", std::move(gpu_lines), sky),
             panel("RAM / short-term memory", std::move(ram_lines), coral)}),
       separator(),
       text("  Temperatures and live clock speeds live in CPU details.  "
            "Process memory lives in Processes.") |
           color(Color::RGB(142, 159, 174))});
}

Elements cpu_details(const Snapshot &snapshot, size_t offset, size_t visible) {
  const Color mint = Color::RGB(94, 218, 177);
  const Color gold = Color::RGB(244, 202, 112);
  Elements rows;
  const CpuTimes &overall = snapshot.raw_cpu.overall;
  rows.push_back(text("Overall CPU counters") | bold | color(mint));
  rows.push_back(text("user " + std::to_string(overall.user) + "  nice " +
                      std::to_string(overall.nice) + "  system " +
                      std::to_string(overall.system) + "  idle " +
                      std::to_string(overall.idle) + "  wait " +
                      std::to_string(overall.iowait) + "  irq " +
                      std::to_string(overall.irq) + "  softirq " +
                      std::to_string(overall.softirq) + "  steal " +
                      std::to_string(overall.steal)) |
                 color(Color::RGB(159, 174, 188)));
  rows.push_back(text("Per-core activity") | bold | color(mint));
  for (size_t index = offset;
       index < snapshot.raw_cpu.cores.size() && rows.size() < visible;
       ++index) {
    const double usage =
        index < snapshot.core_usage.size() ? snapshot.core_usage[index] : 0;
    rows.push_back(hbox(
        {text("core " + std::to_string(index)) |
             color(Color::RGB(126, 148, 166)),
         text("  " + number(usage) + "%") | bold | color(mint),
         text("  user " + std::to_string(snapshot.raw_cpu.cores[index].user) +
              "  nice " + std::to_string(snapshot.raw_cpu.cores[index].nice) +
              "  system " +
              std::to_string(snapshot.raw_cpu.cores[index].system) + "  idle " +
              std::to_string(snapshot.raw_cpu.cores[index].idle) + "  wait " +
              std::to_string(snapshot.raw_cpu.cores[index].iowait) + "  irq " +
              std::to_string(snapshot.raw_cpu.cores[index].irq) + "  softirq " +
              std::to_string(snapshot.raw_cpu.cores[index].softirq) +
              "  steal " +
              std::to_string(snapshot.raw_cpu.cores[index].steal)) |
             color(Color::RGB(159, 174, 188))}));
  }

  rows.push_back(separator());
  rows.push_back(text("Thermal sensors") | bold | color(gold));
  if (snapshot.cpu.temperatures.empty()) {
    rows.push_back(text("No readable thermal zones") |
                   color(Color::RGB(159, 174, 188)));
  } else {
    for (const auto &sensor : snapshot.cpu.temperatures) {
      rows.push_back(labeled_value(sensor.sensor_name,
                                   number(sensor.celsius) + " C", gold));
    }
  }

  rows.push_back(separator());
  rows.push_back(text("Live frequencies") | bold |
                 color(Color::RGB(111, 190, 245)));
  if (snapshot.cpu.frequencies.empty()) {
    rows.push_back(text("Per-core frequency interface unavailable") |
                   color(Color::RGB(159, 174, 188)));
  } else {
    for (const auto &frequency : snapshot.cpu.frequencies) {
      rows.push_back(labeled_value("CPU " + std::to_string(frequency.cpu),
                                   number(frequency.mhz, 0) + " MHz",
                                   Color::RGB(111, 190, 245)));
    }
  }
  return rows;
}

Elements process_rows(const Snapshot &snapshot, size_t offset, size_t visible) {
  std::vector<ProcessMemoryData> processes = snapshot.ram.processes;
  std::sort(processes.begin(), processes.end(),
            [](const ProcessMemoryData &left, const ProcessMemoryData &right) {
              return left.memory_mib > right.memory_mib;
            });

  Elements rows = {
      hbox({text(" PID     PROCESS") | bold | color(Color::RGB(126, 148, 166)),
            filler(),
            text("RSS MiB") | bold | color(Color::RGB(126, 148, 166))}),
      separator()};
  const size_t end = std::min(processes.size(), offset + visible);
  for (size_t index = offset; index < end; ++index) {
    std::string name = processes[index].process_name;
    if (name.size() > 52)
      name.resize(49), name += "...";
    std::ostringstream pid;
    pid << std::setw(6) << processes[index].pid;
    rows.push_back(hbox({text(" " + pid.str() + "  " + name), filler(),
                         text(number(processes[index].memory_mib)) |
                             color(Color::RGB(94, 218, 177))}));
  }
  if (processes.empty())
    rows.push_back(text("No process data available"));
  return rows;
}

} // namespace

int main() {
  if (!init()) {
    std::cerr
        << "NVIDIA management library unavailable; cannot read GPU data.\n";
    return 1;
  }

  Snapshot snapshot;
  refresh(snapshot, snapshot.raw_cpu, false);

  int selected_view = 0;
  size_t process_offset = 0;
  size_t cpu_offset = 0;
  const std::vector<std::string> tabs = {"Overview", "CPU details",
                                         "Processes"};
  ScreenInteractive screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running{true};
  std::thread refresh_thread([&] {
    while (running.load()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      if (running.load())
        screen.PostEvent(Event::Custom);
    }
  });

  Component app = Renderer([&] {
    const auto terminal = Terminal::Size();
    const size_t visible_rows =
        static_cast<size_t>(std::max(1, terminal.dimy - 9));
    Elements tab_elements;
    for (size_t index = 0; index < tabs.size(); ++index) {
      Element tab =
          text(" " + std::to_string(index + 1) + " " + tabs[index] + " ");
      if (static_cast<int>(index) == selected_view)
        tab = tab | bold | color(Color::RGB(30, 38, 46)) |
              bgcolor(Color::RGB(94, 218, 177));
      else
        tab = tab | color(Color::RGB(159, 174, 188));
      tab_elements.push_back(tab);
    }

    Element body;
    if (selected_view == 0) {
      body = overview(snapshot);
    } else if (selected_view == 1) {
      body = panel("CPU / instrument panel",
                   cpu_details(snapshot, cpu_offset, visible_rows),
                   Color::RGB(94, 218, 177));
    } else {
      const size_t process_visible = visible_rows > 2 ? visible_rows - 2 : 1;
      body = panel("Processes / sorted by resident memory",
                   process_rows(snapshot, process_offset, process_visible),
                   Color::RGB(255, 154, 129));
    }

    return vbox(
        {hbox({text(" uniqtop ") | bold | color(Color::RGB(94, 218, 177)),
               text(":: tiny machine observatory") |
                   color(Color::RGB(159, 174, 188)),
               filler(),
               text("LIVE  " + snapshot.updated_at) | bold |
                   color(Color::RGB(244, 202, 112))}),
         separator(), hbox(std::move(tab_elements)), separator(), body,
         filler(), separator(),
         text("  1-3 switch view   Up/Down or PgUp/PgDn scroll   q/Esc quit") |
             color(Color::RGB(126, 148, 166))});
  });

  app = CatchEvent(app, [&](Event event) {
    if (event == Event::Character('q') || event == Event::Escape) {
      screen.Exit();
      return true;
    }
    if (event == Event::Character('1') || event == Event::Character('2') ||
        event == Event::Character('3')) {
      selected_view = event == Event::Character('1')
                          ? 0
                          : (event == Event::Character('2') ? 1 : 2);
      return true;
    }
    if (event == Event::ArrowDown || event == Event::PageDown) {
      size_t &offset = selected_view == 1 ? cpu_offset : process_offset;
      offset += event == Event::PageDown ? 10 : 1;
      return true;
    }
    if (event == Event::ArrowUp || event == Event::PageUp) {
      size_t &offset = selected_view == 1 ? cpu_offset : process_offset;
      const size_t step = event == Event::PageUp ? 10 : 1;
      offset = offset > step ? offset - step : 0;
      return true;
    }
    if (event == Event::Custom) {
      const AllCpuTimes old_cpu = snapshot.raw_cpu;
      refresh(snapshot, old_cpu, true);
      cpu_offset = std::min(cpu_offset, snapshot.raw_cpu.cores.size());
      process_offset = std::min(process_offset, snapshot.ram.processes.size());
      return true;
    }
    return false;
  });

  screen.Loop(app);
  running.store(false);
  refresh_thread.join();
  return 0;
}