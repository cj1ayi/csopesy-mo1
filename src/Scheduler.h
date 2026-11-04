#pragma once

#include "Config.h"
#include "Process.h"
#include "ProgramGenerator.h"

#include <memory>
#include <string>
#include <vector>

class Scheduler {
  public:
    explicit Scheduler(const Config &config);

    void start_batch(); // scheduler-start
    void stop_batch();  // scheduler-stop

    std::shared_ptr<Process> create_process(const std::string &name); // nullptr if the name is taken
    std::shared_ptr<Process> find_process(const std::string &name);   // nullptr if missing or finished

    std::string status_report(); // shared by screen -ls and report-util

  private:
    Config config;
    ProgramGenerator generator;
    std::vector<std::shared_ptr<Process>> processes;
    std::unordered_map<std::string, std::shared_ptr<Process>> name_to_process;
    int next_pid = 1;
};
