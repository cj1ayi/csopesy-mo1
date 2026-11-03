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
    uint16_t loop_count = 0;
};

struct Instruction {
    OPCODE opcode;
    Argument argument;
};

struct Frame {
    std::vector<Instruction> instructions;
    uint16_t pc;
    int64_t loop_count;
};

class Process {
private:
    std::string name;
    std::vector<Frame> call_stack;
    std::vector<std::string> print_logs;
    std::unordered_map<std::string, uint16_t> variables;
    STATUS status;
    uint8_t sleeps;
    int core_id = -1; // take inspo from linux rahh

    const Instruction *fetch();
    void execute(const Instruction &instruction);

public:
    Process(std::string name, std::vector<Instruction> instructions);
    STATUS tick();
};
