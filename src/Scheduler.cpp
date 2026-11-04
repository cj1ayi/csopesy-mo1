#include "Scheduler.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace {
std::string format_time(const std::chrono::system_clock::time_point &tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &t);
#else
    localtime_r(&t, &local);
#endif
    std::ostringstream out;
    out << std::put_time(&local, "(%m/%d/%Y %I:%M:%S%p)");
    return out.str();
}
} // namespace

Scheduler::Scheduler(const Config &config) : config(config), generator(config.min_ins, config.max_ins) {}

void Scheduler::start_batch() {}

void Scheduler::stop_batch() {}

std::shared_ptr<Process> Scheduler::create_process(const std::string &name) {
    if (find_process(name)) return nullptr;

    auto process = std::make_shared<Process>(next_pid++, name, generator.generate());
    processes.push_back(process);
    return process;
}

std::shared_ptr<Process> Scheduler::find_process(const std::string &name) {
    for (const auto &process : processes)
        if (process->name == name && process->status != TERMINATED) return process;
    return nullptr;
}

std::string Scheduler::status_report() {
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
