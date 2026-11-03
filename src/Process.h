#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

enum STATUS {
    SLEEPING,
    READY,
    TERMINATED
};

enum OPCODE {
    PRINT,
    DECLARE,
    ADD,
    SUBTRACT,
    SLEEP,
    FOR
};

struct Instruction;

// one struct to rule them all
// hacky/cheesy/inefficient lol but simple
struct Argument {
    // DECLARE
    std::string var_name;
    uint16_t value = 0;

    // ADD / SUBTRACT
    int arg_config = 0; // 00 for both immediates, 01 for var1 imm, 10 for var2 imm, 11 for both vars
    std::string var1_name;
    std::string var2_name;
    uint16_t var2_value = 0;
    std::string var3_name;
    uint16_t var3_value = 0;

    // SLEEP
    uint8_t sleep_arg = 0; // number of CPU ticks

    // FOR
    std::vector<Instruction> body;
    uint32_t loop_count = 0;
};

struct Instruction {
    OPCODE opcode;
    Argument argument;
};

struct Frame {
    std::vector<Instruction> instructions;
    uint64_t pc;
    int64_t loop_count;
};

class Process {
private:
    static uint64_t count_loc(const std::vector<Instruction> &instructions);

    std::vector<Frame> call_stack;
    std::unordered_map<std::string, uint16_t> variables;

    const Instruction *fetch();
    void execute(const Instruction &instruction);

public:
    int pid;
    std::string name;
    std::vector<std::string> print_logs;
    STATUS status;
    uint8_t sleeps;
    int core_id = -1; // take inspo from linux rahh

    const uint64_t loc;
    uint64_t executes = 0;

    const std::chrono::system_clock::time_point created_at = std::chrono::system_clock::now();

    Process(int pid, std::string name, std::vector<Instruction> instructions);
    STATUS tick();
};
