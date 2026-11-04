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

    if (it == name_to_process.end()) { return nullptr; }

    auto p = it->second;
    std::lock_guard<std::mutex> lock(p->mtx);
    if (p->status == TERMINATED) return nullptr;

    return p;
}

std::string ProcessManager::status_report() {
    std::vector<std::shared_ptr<Process>> snapshot;
    {
        std::lock_guard<std::mutex> lock(mtx);
        snapshot = processes;
    }

    uint32_t cores_used = 0;
    std::ostringstream running, finished;
    for (const auto &process : snapshot) {
        STATUS status;
        int core_id;
        uint64_t executes;
        {
            std::lock_guard<std::mutex> process_lock(process->mtx);
            status = process->status;
            core_id = process->core_id;
            executes = process->executes;
        }

        if (status == TERMINATED) {
            finished << std::left << std::setw(12) << process->name << format_time(process->created_at)
                     << "    Finished    " << executes << " / " << process->loc << "\n";
        } else if (core_id != -1) {
            ++cores_used;
            running << std::left << std::setw(12) << process->name << format_time(process->created_at)
                    << "    Core: " << std::setw(6) << core_id << executes << " / " << process->loc << "\n";
        }
    }

    std::ostringstream out;
    out << "CPU utilization: " << (cores_used * 100 / config.num_cpu) << "%\n";
    out << "Cores used: " << cores_used << "\n";
    out << "Cores available: " << (config.num_cpu - cores_used) << "\n\n";
    out << "----------------------------------------\n";
    out << "Running processes:\n" << running.str();
    out << "\nFinished processes:\n" << finished.str();
    out << "----------------------------------------\n";
    return out.str();
}
