#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

class Process {
private:
    std::string name;
    uint16_t pc;
    InstructionList instructions;
    std::vector<std::string> print_logs;
    std::unordered_map<std::string, uint16_t> variables;
    STATUS status;
    uint8_t sleep_ticks_remaining;

public:
    Process(std::string name, InstructionList instructions);
    void run();
};

typedef enum STATUS {
    RUNNING,
    SLEEPING,
    READY,
    TERMINATED
} STATUS;

typedef enum OPCODE {
    PRINT,
    DECLARE,
    ADD,
    SUBTRACT,
    SLEEP,
    FOR
} OPCODE;

typedef union ARGUMENT {
    struct {
        std::string var_name;
        uint16_t value;
    } declare_arg;

    struct {
        int arg_config; // 00 for both immediates, 01 for var1 imm, 10 for var2 imm, 11 for both vars
        std::string var1_name;
        union {
            std::string var2_name;
            uint16_t value;
        } var2;
        union  {
            std::string var3_name;
            uint16_t value;
        } var3;
    } add_arg;

    // no subtract_arg, just reuse add_arg

    uint8_t sleep_arg; // no. of cpu ticks

    struct {
        InstructionList instructions;
        uint16_t loop_count;
    } for_arg;

} ARGUMENT;

typedef struct Instruction
{
    OPCODE opcode;
    ARGUMENT argument;
} Instruction;

typedef struct InstructionList {
    Instruction* instructions;
    uint16_t instruction_count;
} InstructionList;