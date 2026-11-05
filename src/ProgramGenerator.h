#pragma once

#include "Instruction.h"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

class ProgramGenerator {
  public:
    ProgramGenerator(uint64_t min_ins, uint64_t max_ins, uint64_t seed = std::random_device{}());

    std::vector<Instruction> generate(const std::string &process_name);

  private:
    std::mt19937 rng;
    const uint64_t min_ins, max_ins;

    Instruction make_print();
    Instruction make_add();

    uint64_t rand_in(uint64_t lo, uint64_t hi);
};
