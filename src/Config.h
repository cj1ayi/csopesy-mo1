#pragma once

#include <cstdint>
#include <string>

enum SCHEDULER_ALGORITHM { FCFS, RR };


struct Config {
    uint32_t num_cpu = 4;
    SCHEDULER_ALGORITHM scheduler = RR;
    uint64_t quantum_cycles = 5;
    uint64_t batch_process_freq = 1;
    uint64_t min_ins = 1000;
    uint64_t max_ins = 2000;
    uint64_t delay_per_exec = 0;

    // throws std::runtime_error on missing file, unknown key or out-of-range value
    static Config load(const std::string &path);
};
