#pragma once

#include "Instruction.h"

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

enum STATUS { SLEEPING, READY, TERMINATED };

struct Frame {
    std::vector<Instruction> instructions;
    uint64_t pc;
    int64_t loop_count;
};

class Process {
  private:
    std::vector<Frame> call_stack;
    std::unordered_map<std::string, uint16_t> variables;

    const Instruction *fetch();
    void execute(const Instruction &instruction);

  public:
    int pid;
    std::string name;
    std::vector<std::string> print_logs; // need mutex
    STATUS status;                       // need mutex
    uint8_t sleeps;                      // need mutex
    int core_id = -1;                    // take inspo from linux rahh // need mutex

    const uint64_t loc;
    uint64_t executes = 0; // need mutex

    const std::chrono::system_clock::time_point created_at = std::chrono::system_clock::now();

    Process(int pid, std::string name, std::vector<Instruction> instructions);
    STATUS tick();

    mutable std::mutex mtx;
};
