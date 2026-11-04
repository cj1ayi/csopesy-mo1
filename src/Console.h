#pragma once

#include "Config.h"
#include "ProcessManager.h"

#include <memory>
#include <string>
#include <vector>

class Console {
  public:
    void run();

  private:
    std::unique_ptr<ProcessManager> process_manager; // null until "initialize" succeeds

    void print_header();
    void initialize();
    void screen(const std::vector<std::string> &args);
    void report_util();

    void attach(const std::shared_ptr<Process> &process);
    void print_process_smi(const Process &process);
};
