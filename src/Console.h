#pragma once

#include "Config.h"
#include "Scheduler.h"

#include <memory>
#include <string>
#include <vector>

class Console {
  public:
    void run();

  private:
    std::unique_ptr<Scheduler> scheduler; // null until "initialize" succeeds

    void print_header();
    void initialize();
    void screen(const std::vector<std::string> &args);
    void report_util();

    void attach(const std::shared_ptr<Process> &process);
    void print_process_smi(const Process &process);
};
