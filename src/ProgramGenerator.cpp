#include "ProgramGenerator.h"

#include <stdexcept>

ProgramGenerator::ProgramGenerator(const uint64_t min_ins, const uint64_t max_ins, const uint64_t seed)
    : rng(seed), min_ins(min_ins), max_ins(max_ins) {
    if (min_ins == 0 || min_ins > max_ins) throw std::invalid_argument("require 1 <= min_ins <= max_ins");
}

// always [PRINT, ADD, PRINT, ADD, ...] over x, which reads as 0 until the first ADD
std::vector<Instruction> ProgramGenerator::generate(const std::string & /*process_name*/) {
    const uint64_t count = rand_in(min_ins, max_ins);

    std::vector<Instruction> out;
    out.reserve(count);
    for (uint64_t i = 0; i < count; ++i) out.push_back(i % 2 == 0 ? make_print() : make_add());
    return out;
}

Instruction ProgramGenerator::make_print() {
    Instruction instruction{PRINT, {}};
    instruction.argument.msg = "Value from: %d";
    instruction.argument.msg_vars = {"x"};
    return instruction;
}

Instruction ProgramGenerator::make_add() {
    Instruction instruction{ADD, {}};
    auto &arg = instruction.argument;
    arg.arg_config = 0b10; // var2 is a variable, var3 is an immediate
    arg.var1_name = "x";
    arg.var2_name = "x";
    arg.var3_value = static_cast<uint16_t>(rand_in(1, 10));
    return instruction;
}

uint64_t ProgramGenerator::rand_in(const uint64_t lo, const uint64_t hi) {
    return std::uniform_int_distribution<uint64_t>(lo, hi)(rng);
}
