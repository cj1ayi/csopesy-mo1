#pragma once

#include "Config.h"
#include "Process.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

struct Core {
    std::shared_ptr<Process> process; // nullptr when idle
    uint64_t busywait_left = 0;       // ticks left before the next instruction
    uint64_t ticks = 0;               // ticks spent in the current time slice
};

class Scheduler {
  public:
    explicit Scheduler(const Config &config);
    virtual ~Scheduler() = default;

    void add(const std::shared_ptr<Process> &process);
    void tick();

    // no tick runs while the returned lock is held, so process state stays consistent across processes
    std::unique_lock<std::mutex> pause();

  protected:
    const Config config;

    virtual bool should_preempt(const Core &core) const = 0;

  private:
    std::vector<Core> cores;
    std::queue<std::shared_ptr<Process>> ready;
    std::queue<std::shared_ptr<Process>> sleeping;
    std::mutex ready_mtx;
    std::mutex tick_mtx;

    void wake_sleepers();
    void run_cores();
    void dispatch();
    static void release(Core &core);
};

class FCFSScheduler : public Scheduler {
  public:
    using Scheduler::Scheduler;

  protected:
    bool should_preempt(const Core &) const override { return false; }
};

class RRScheduler : public Scheduler {
  public:
    using Scheduler::Scheduler;

  protected:
    bool should_preempt(const Core &core) const override { return core.ticks >= config.quantum_cycles; }
};

std::unique_ptr<Scheduler> make_scheduler(const Config &config);
