#pragma once

#include "Instruction.h"

#include <cstdint>
#include <random>
#include <vector>

class ProgramGenerator {
  public:
    static constexpr int MAX_FOR_DEPTH = 3;
    static constexpr int VARIABLE_POOL_SIZE = 256;

    ProgramGenerator(uint64_t min_ins, uint64_t max_ins, uint64_t seed = std::random_device{}());

    std::vector<Instruction> generate();

  private:
    std::mt19937 rng;
    uint64_t min_ins, max_ins;

    std::vector<Instruction> gen(uint64_t budget, const int depth);
    Instruction fill_instruction(const OPCODE opcode);

    uint64_t rand_in(const uint64_t lo, const uint64_t hi);
    std::string random_var();
};
