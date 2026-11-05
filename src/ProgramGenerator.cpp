#include "ProgramGenerator.h"

#include <limits>
#include <stdexcept>
#include <utility>

ProgramGenerator::ProgramGenerator(const uint64_t min_ins, const uint64_t max_ins, const uint64_t seed)
    : rng(seed), min_ins(min_ins), max_ins(max_ins) {
    if (min_ins == 0 || min_ins > max_ins) throw std::invalid_argument("require 1 <= min_ins <= max_ins");
}

std::vector<Instruction> ProgramGenerator::generate(const std::string &process_name) {
    // msg is a format string, so a % in the process name must not be read as a conversion
    std::string print_msg = "Hello world from ";
    for (const char c : process_name) {
        if (c == '%') print_msg += '%';
        print_msg += c;
    }
    print_msg += '!';

    return gen(rand_in(min_ins, max_ins), 0, print_msg);
}

std::vector<Instruction> ProgramGenerator::gen(uint64_t budget, const int depth, const std::string &print_msg) {
    std::vector<Instruction> out;
    while (budget > 0) {
        const bool can_loop = depth < MAX_FOR_DEPTH && budget >= 1;
        const auto opcode = static_cast<OPCODE>(rand_in(PRINT, can_loop ? FOR : SLEEP));
        if (opcode == FOR) {
            // loop count k >= 1 and body size b >= 1 with k * b <= budget
            const uint64_t k = rand_in(1, budget);
            const uint64_t b = rand_in(1, budget / k);

            Instruction instruction{FOR, {}};
            instruction.argument.loop_count = static_cast<uint32_t>(k);
            instruction.argument.body = gen(b, depth + 1, print_msg);
            out.push_back(std::move(instruction));
            budget -= k * b;
        } else {
            out.push_back(fill_instruction(opcode, print_msg));
            budget -= 1;
        }
    }
    return out;
}

// doesn't fill in FOR btw
Instruction ProgramGenerator::fill_instruction(const OPCODE opcode, const std::string &print_msg) {
    constexpr auto U16_MAX = std::numeric_limits<uint16_t>::max();

    Instruction instruction{opcode, {}};
    auto &arg = instruction.argument;
    switch (instruction.opcode) {
        case PRINT:
            arg.msg = print_msg;
            break;
        case DECLARE:
            arg.var_name = random_var();
            arg.value = static_cast<uint16_t>(rand_in(0, U16_MAX));
            break;
        case ADD:
        case SUBTRACT:
            arg.arg_config = static_cast<int>(rand_in(0, 0b11));
            arg.var1_name = random_var();
            if (arg.arg_config & 0b10)
                arg.var2_name = random_var();
            else
                arg.var2_value = static_cast<uint16_t>(rand_in(0, U16_MAX));
            if (arg.arg_config & 0b01)
                arg.var3_name = random_var();
            else
                arg.var3_value = static_cast<uint16_t>(rand_in(0, U16_MAX));
            break;
        case SLEEP:
            arg.sleep_arg = static_cast<uint8_t>(rand_in(1, std::numeric_limits<uint8_t>::max()));
            break;
        case FOR:
            // should never be reached
            break;
    }
    return instruction;
}

uint64_t ProgramGenerator::rand_in(const uint64_t lo, const uint64_t hi) {
    return std::uniform_int_distribution<uint64_t>(lo, hi)(rng);
}

std::string ProgramGenerator::random_var() { return "x" + std::to_string(rand_in(0, VARIABLE_POOL_SIZE - 1)); }
