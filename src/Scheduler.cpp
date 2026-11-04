#include "Scheduler.h"

Scheduler::Scheduler(const Config &config) : config(config), cores(config.num_cpu) {}

void Scheduler::add(const std::shared_ptr<Process> &process) {
    std::lock_guard<std::mutex> lock(ready_mtx);
    ready.push(process);
}

void Scheduler::tick() {
    wake_sleepers();
    run_cores();
    dispatch();
}

void Scheduler::wake_sleepers() {
    for (size_t remaining = sleeping.size(); remaining > 0; --remaining) {
        auto process = sleeping.front();
        sleeping.pop();

        bool awake;
        {
            std::lock_guard<std::mutex> lock(process->mtx);
            awake = --process->sleeps == 0;
            if (awake) process->status = READY;
        }

        if (awake)
            add(process);
        else
            sleeping.push(process);
    }
}

void Scheduler::run_cores() {
    for (auto &core : cores) {
        if (!core.process) continue;

        auto process = core.process;
        STATUS status = READY;
        if (core.busywait_left > 0) {
            --core.busywait_left;
        } else {
            status = process->tick();
            core.busywait_left = config.delay_per_exec;
        }
        ++core.ticks;

        if (status == TERMINATED) {
            release(core);
        } else if (status == SLEEPING) {
            release(core);
            sleeping.push(process);
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
        core.busywait_left = 0;
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
