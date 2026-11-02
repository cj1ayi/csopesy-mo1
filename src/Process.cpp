#include "Process.h"

Process::Process(std::string name, InstructionList instructions) : name(name), pc(0), instructions(instructions), status(READY), sleep_ticks_remaining(0) {}

void Process::run() {
    while (pc < instructions.instruction_count) {
        const auto& current_instruction = instructions.instructions[pc];
        const auto& op = current_instruction.opcode;
        const auto& arg = current_instruction.argument;
    
        switch (op) {
            case PRINT: {
                std::string log;
                const auto var = name;
                constexpr auto format_string = "Hello world from %s!";
                log.resize(std::strlen(format_string) + var.size() - 2);
                int written = std::snprintf(&log[0], log.size() + 1, format_string, var.c_str());
                print_logs.push_back(log);
                break;
            }
            case DECLARE: {
                const auto var_name = arg.declare_arg.var_name;
                const auto value = arg.declare_arg.value;
                variables[var_name] = value;
                break;
            }
            case SUBTRACT:
            case ADD: {
                uint16_t var2_value, var3_value;
                if (arg.add_arg.arg_config & 0b10) // var2 is not immediate
                {
                    // if var2 doesn't exist, declare with 0
                    if (variables.find(arg.add_arg.var2.var2_name) == variables.end())
                    {
                        variables[arg.add_arg.var2.var2_name] = 0;
                    }
                    var2_value = variables[arg.add_arg.var2.var2_name];
                } else var2_value = arg.add_arg.var2.value;
                if (arg.add_arg.arg_config & 0b01) // var3 is not immediate
                {
                    // if var3 doesn't exist, declare with 0
                    if (variables.find(arg.add_arg.var3.var3_name) == variables.end())
                    {
                        variables[arg.add_arg.var3.var3_name] = 0;
                    }
                    var3_value = variables[arg.add_arg.var3.var3_name];
                } else var3_value = arg.add_arg.var3.value;

                if (op == SUBTRACT)
                    var3_value = -var3_value;
                
                variables[arg.add_arg.var1_name] = var2_value + var3_value;
                break;
            }
            case SLEEP:
                sleep_ticks_remaining = arg.sleep_arg;
                status = SLEEPING;
                break;
            case FOR:
                // Handle FOR instruction
                break;
            default:
                // Handle unknown opcode
                break;
        }
        pc++;
    }
}