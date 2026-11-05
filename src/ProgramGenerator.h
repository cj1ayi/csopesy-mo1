#pragma once

#include "Instruction.h"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

class ProgramGenerator {
  public:
    static constexpr int MAX_FOR_DEPTH = 3;
    static constexpr int VARIABLE_POOL_SIZE = 256;

    ProgramGenerator(uint64_t min_ins, uint64_t max_ins, uint64_t seed = std::random_device{}());

    std::vector<Instruction> generate(const std::string &process_name);

  private:
    std::mt19937 rng;
    const uint64_t min_ins, max_ins;

    std::vector<Instruction> gen(uint64_t budget, int depth, const std::string &print_msg);
    Instruction fill_instruction(OPCODE opcode, const std::string &print_msg);

    uint64_t rand_in(uint64_t lo, uint64_t hi);
    std::string random_var();
};
