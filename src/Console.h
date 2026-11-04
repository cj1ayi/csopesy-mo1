#pragma once

#include "Config.h"
#include "ProcessManager.h"
#include "Scheduler.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class Console {
  public:
    ~Console();

    void run();

  private:
    std::unique_ptr<ProcessManager> process_manager; // null until "initialize" succeeds
    std::unique_ptr<Scheduler> scheduler;            // null until "initialize" succeeds
    Config config;

    std::thread tick_thread; // runs from "initialize" until exit
    std::atomic<bool> running{false};
    std::atomic<bool> batch_enabled{false};
    uint64_t batch_count = 0; // only touched by tick_thread

    std::shared_ptr<Process> spawn(const std::string &name);
    std::string next_batch_name();
    void tick_loop();

    void print_header();
    void initialize();
    void screen(const std::vector<std::string> &args);
    void report_util();
    std::string status_report(); // shared by screen -ls and report-util
    std::string format_status_report(const std::vector<ProcessSnapshot> &snapshot) const;

    void attach(const std::shared_ptr<Process> &process);
    void print_process_smi(const Process &process);
};
