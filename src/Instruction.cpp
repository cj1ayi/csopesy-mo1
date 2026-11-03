#include "Instruction.h"

uint64_t count_loc(const std::vector<Instruction> &instructions) {
    uint64_t loc = 0;
    for (const auto &instruction : instructions) {
        if (instruction.opcode == FOR)
            loc += static_cast<uint64_t>(instruction.argument.loop_count) * count_loc(instruction.argument.body);
        else
            ++loc;
    }
    return loc;
}
