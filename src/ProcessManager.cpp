#include "ProcessManager.h"

ProcessManager::ProcessManager(const Config &config) : generator(config.min_ins, config.max_ins) {}

std::shared_ptr<Process> ProcessManager::create_process(const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx);

    if (_find_process(name)) return nullptr;

    auto process = std::make_shared<Process>(next_pid++, name, generator.generate(name));
    processes.push_back(process);
    name_to_process[name] = process;
    return process;
}

std::shared_ptr<Process> ProcessManager::find_process(const std::string &name) const {
    std::lock_guard<std::mutex> lock(mtx);
    return _find_process(name);
}

std::shared_ptr<Process> ProcessManager::_find_process(const std::string &name) const {
    const auto it = name_to_process.find(name);

    if (it == name_to_process.end()) { return nullptr; }

    auto p = it->second;
    std::lock_guard<std::mutex> lock(p->mtx);
    if (p->status == TERMINATED) return nullptr;

    return p;
}

std::vector<ProcessSnapshot> ProcessManager::snapshot() const {
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<ProcessSnapshot> result;
    result.reserve(processes.size());
    for (const auto &process : processes) {
        std::lock_guard<std::mutex> process_lock(process->mtx);
        result.push_back({process, process->status, process->core_id, process->executes});
    }
    return result;
}
