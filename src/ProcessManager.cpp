#include "ProcessManager.h"

#include "Utils.h"

#include <iomanip>
#include <sstream>

ProcessManager::ProcessManager(const Config &config) : config(config), generator(config.min_ins, config.max_ins) {}

std::shared_ptr<Process> ProcessManager::create_process(const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx);

    if (_find_process(name)) return nullptr;

    auto process = std::make_shared<Process>(next_pid++, name, generator.generate());
    processes.push_back(process);
    name_to_process[name] = process;
    return process;
}

std::shared_ptr<Process> ProcessManager::find_process(const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx);
    return _find_process(name);
}

std::shared_ptr<Process> ProcessManager::_find_process(const std::string &name) {
    auto it = name_to_process.find(name);

    if (it == name_to_process.end() || it->second->status == TERMINATED) return nullptr;

    return it->second;
}

std::string ProcessManager::status_report() {
    std::lock_guard<std::mutex> lock(mtx);

    uint32_t cores_used = 0;
    for (const auto &process : processes)
        if (process->status != TERMINATED && process->core_id != -1) ++cores_used;

    std::ostringstream out;
    out << "CPU utilization: " << (cores_used * 100 / config.num_cpu) << "%\n";
    out << "Cores used: " << cores_used << "\n";
    out << "Cores available: " << (config.num_cpu - cores_used) << "\n\n";
    out << "----------------------------------------\n";

    out << "Running processes:\n";
    for (const auto &process : processes) {
        if (process->status == TERMINATED || process->core_id == -1) continue;
        out << std::left << std::setw(12) << process->name << format_time(process->created_at)
            << "    Core: " << std::setw(6) << process->core_id << process->executes << " / " << process->loc << "\n";
    }

    out << "\nFinished processes:\n";
    for (const auto &process : processes) {
        if (process->status != TERMINATED) continue;
        out << std::left << std::setw(12) << process->name << format_time(process->created_at) << "    Finished    "
            << process->executes << " / " << process->loc << "\n";
    }
    out << "----------------------------------------\n";
    return out.str();
}
