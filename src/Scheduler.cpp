#include "Scheduler.h"

Scheduler::Scheduler(const Config &config) : config(config), cores(config.num_cpu) {}

void Scheduler::add(const std::shared_ptr<Process> &process) {
    std::lock_guard<std::mutex> lock(ready_mtx);
    ready.push(process);
}

void Scheduler::tick() {
    std::lock_guard<std::mutex> lock(tick_mtx);
    ++ticks;
    wake_sleepers();
    run_cores();
    dispatch();
}

std::unique_lock<std::mutex> Scheduler::pause() { return std::unique_lock<std::mutex>(tick_mtx); }

void Scheduler::wake_sleepers() {
    const auto wheel_pos = ticks % sleep_wheel.size();

    auto &wake_now = sleep_wheel[wheel_pos];
    for (const auto &process : wake_now) {
        {
            std::lock_guard<std::mutex> lock(process->mtx);
            process->status = READY;
        }
        add(process);
    }
    wake_now.clear();
}

void Scheduler::run_cores() {
    for (auto &core : cores) {
        if (!core.process) continue;

        const auto process = core.process;
        STATUS status = READY;
        if (process->busywait_left > 0) {
            --process->busywait_left;
        } else {
            status = process->tick();
            process->busywait_left = config.delay_per_exec;
        }
        ++core.ticks;

        if (status == TERMINATED) {
            release(core);
        } else if (status == SLEEPING) {
            release(core);
            sleep_wheel[(ticks + process->sleeps) % sleep_wheel.size()].push_back(process);
        } else if (should_preempt(core)) {
            release(core);
            add(process);
        }
    }
}

void Scheduler::dispatch() {
    for (size_t id = 0; id < cores.size(); ++id) {
        auto &core = cores[id];
        if (core.process) continue;

        std::shared_ptr<Process> next;
        {
            std::lock_guard<std::mutex> lock(ready_mtx);
            if (ready.empty()) return;
            next = ready.front();
            ready.pop();
        }

        {
            std::lock_guard<std::mutex> lock(next->mtx);
            next->core_id = static_cast<int>(id);
        }
        core.process = next;
        core.ticks = 0;
    }
}

void Scheduler::release(Core &core) {
    {
        std::lock_guard<std::mutex> lock(core.process->mtx);
        core.process->core_id = -1;
    }
    core.process = nullptr;
}

std::unique_ptr<Scheduler> make_scheduler(const Config &config) {
    if (config.scheduler == FCFS) return std::make_unique<FCFSScheduler>(config);
    return std::make_unique<RRScheduler>(config);
}
