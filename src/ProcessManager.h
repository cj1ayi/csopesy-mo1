#pragma once

#include "Config.h"
#include "Process.h"
#include "ProgramGenerator.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

struct ProcessSnapshot {
    std::shared_ptr<Process> process; // for the fields that never change
    STATUS status;
    int core_id;
    uint64_t executes;
};

class ProcessManager {
  public:
    explicit ProcessManager(const Config &config);

    std::shared_ptr<Process> create_process(const std::string &name); // nullptr if the name is taken
    std::shared_ptr<Process> find_process(const std::string &name);   // nullptr if missing or finished

    std::vector<ProcessSnapshot> snapshot();

  private:
    ProgramGenerator generator;
    std::vector<std::shared_ptr<Process>> processes;
    std::unordered_map<std::string, std::shared_ptr<Process>> name_to_process;
    int next_pid = 1;

    std::mutex mtx; // for processes and name_to_process

    std::shared_ptr<Process> _find_process(const std::string &name);
};
