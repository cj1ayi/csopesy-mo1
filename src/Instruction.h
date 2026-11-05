#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum OPCODE { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

struct Instruction;

// one struct to rule them all
// hacky/cheesy/inefficient lol but simple
struct Argument {
    // PRINT
    std::string msg;                   // C-style format string, e.g. "x = %d, y = %04x"
    std::vector<std::string> msg_vars; // variables consumed by msg's conversions, in order

    // DECLARE
    std::string var_name;
    uint16_t value = 0;

    // ADD / SUBTRACT
    int arg_config = 0; // 00 for both immediates, 01 for var1 imm, 10 for var2
                        // imm, 11 for both vars
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

uint64_t count_loc(const std::vector<Instruction> &instructions);
