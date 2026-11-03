#include "Process.h"
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <chrono>

Process::Process(int pid, std::string name, std::vector<Instruction> instructions) : call_stack({Frame{instructions, 0, 0}}), pid(pid), name(name), status(READY), sleeps(0), loc(count_loc(instructions)) {}

const Instruction *Process::fetch() {
    while (!call_stack.empty()) {
        auto &current_frame = call_stack.back();

        if (current_frame.pc >= current_frame.instructions.size()) {
            if (--current_frame.loop_count > 0)
                current_frame.pc = 0;
            else {
                call_stack.pop_back();
                if (!call_stack.empty())
                    call_stack.back().pc++;
            }
            continue;
        }

        const auto &instruction = current_frame.instructions[current_frame.pc];
        if (instruction.opcode == FOR) {
            const auto &arg = instruction.argument;
            if (arg.loop_count > 0 && !arg.body.empty())
                call_stack.push_back(Frame{arg.body, 0, arg.loop_count});
            else
                current_frame.pc++;
            continue;
        }

        return &instruction;
    }

    return nullptr;
}

void Process::execute(const Instruction &instruction) {
    const auto &op = instruction.opcode;
    const auto &arg = instruction.argument;
    switch (op)
    {
    case PRINT:
    {
        print_logs.push_back("Hello world from " + name + "!");
        break;
    }
        case DECLARE:
            variables[arg.var_name] = arg.value;
            break;
        case SUBTRACT:
        case ADD: {
            int32_t var2_value = (arg.arg_config & 0b10) ? variables[arg.var2_name] : arg.var2_value;
            int32_t var3_value = (arg.arg_config & 0b01) ? variables[arg.var3_name] : arg.var3_value;

            if (op == SUBTRACT)
                var3_value = -var3_value;

            variables[arg.var1_name] = static_cast<uint16_t>(std::clamp(var2_value + var3_value, 0, (1 << 16) - 1));
            break;
        }
        case SLEEP:
            if (arg.sleep_arg > 0) {
                sleeps = arg.sleep_arg;
                status = SLEEPING;
            }
            break;
        case FOR:
            // should never reach
            break;
        default:
            // Handle unknown opcode
            throw std::runtime_error("Unknown opcode encountered");
        }
}

STATUS Process::tick() {
    assert(status == READY);
    if (const Instruction *instruction = fetch())
    {
        execute(*instruction);
        ++executes;

        if (!call_stack.empty())
            call_stack.back().pc++;
    }

    if (status != SLEEPING && !fetch())
        status = TERMINATED;

    return status;
}

uint64_t Process::count_loc(const std::vector<Instruction> &instructions) {
    uint64_t loc = 0;
    for (const auto &instruction : instructions) {
        if (instruction.opcode == FOR)
            loc += static_cast<uint64_t>(instruction.argument.loop_count) * count_loc(instruction.argument.body);
        else
            ++loc;
    }
    return loc;
}