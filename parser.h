#pragma once

#include <optional>
#include <variant>
#include <array>
#include <vector>
#include <unordered_map>
#include <string>

#include "scanner.h"

constexpr size_t INSTRUCTION_IR_OPERAND_SIZE = 3;


//assinging values to insure comparison with TokenCategory
enum class OperandType {
    CONSTANT = 0,
    REGISTER = 1,
    NOTHING
};

enum class OperandLocation {
    OP1,
    OP2,
    DEST,
    NONE
};

enum class PrintType {
    SCRIPT_REGISTER,
    VIRTUAL_REGISTER,
    PHYSICAL_REGISTER
};

struct InstructionIROperand {
    OperandType type{OperandType::NOTHING};
    int value{-1};
    int VR{-1};
    int PR{-1};
    int nextUse{-1};
    int rematValue{-1};
    bool nextUseDirty{true};

    std::string getOperandString(const PrintType pType) const;
    void debugPrint() const;

    InstructionIROperand() {};

    enum class ConstType { Value, PR };

    InstructionIROperand(OperandType type, int value, ConstType initType)
        : type(type) {
        if (initType == ConstType::Value) {
            this->value = value;
        } else {
            this->PR = value;
        }
    }
};

struct InstructionIR {
    Instruction opcode {(Instruction)-1};
    InstructionIROperand op1 = InstructionIROperand();
    InstructionIROperand op2 = InstructionIROperand();
    InstructionIROperand dest = InstructionIROperand();
    void print(const PrintType pType) const;
    void tablePrint() const;
    void debugPrint(int instructionIndex) const;
};

//map of the instruction to read to an array of pairs
//3 elements in the array, one for each of the possible operands in order
//the first element of the array is the expected token to find at this location
//the second element of the array is which InstructionIR member this token's value 
//  should go into
static std::unordered_map<
    Instruction, 
    std::array<
        std::pair<TokenCategory, OperandLocation>, 
        INSTRUCTION_IR_OPERAND_SIZE
    >
> InstructionIRMap {
    {
        Instruction::LOAD, {{
            {TokenCategory::REGISTER, OperandLocation::OP1},
            {TokenCategory::NOTHING, OperandLocation::NONE},
            {TokenCategory::REGISTER, OperandLocation::DEST}
        }}
    },
    {
        Instruction::LOADI, {{
            {TokenCategory::CONSTANT, OperandLocation::OP1},
            {TokenCategory::NOTHING, OperandLocation::NONE},
            {TokenCategory::REGISTER, OperandLocation::DEST}
        }}
    },
    {
        Instruction::STORE, {{
            {TokenCategory::REGISTER, OperandLocation::OP1},
            {TokenCategory::NOTHING, OperandLocation::NONE},
            {TokenCategory::REGISTER, OperandLocation::OP2}
        }}
    },
    {
        Instruction::ADD, {{
            {TokenCategory::REGISTER, OperandLocation::OP1},
            {TokenCategory::REGISTER, OperandLocation::OP2},
            {TokenCategory::REGISTER, OperandLocation::DEST}
        }}
    },
    {
        Instruction::SUB, {{
            {TokenCategory::REGISTER, OperandLocation::OP1},
            {TokenCategory::REGISTER, OperandLocation::OP2},
            {TokenCategory::REGISTER, OperandLocation::DEST}
        }}
    },
    {
        Instruction::MULT, {{
            {TokenCategory::REGISTER, OperandLocation::OP1},
            {TokenCategory::REGISTER, OperandLocation::OP2},
            {TokenCategory::REGISTER, OperandLocation::DEST}
        }}
    },
    {
        Instruction::LSHIFT, {{
            {TokenCategory::REGISTER, OperandLocation::OP1},
            {TokenCategory::REGISTER, OperandLocation::OP2},
            {TokenCategory::REGISTER, OperandLocation::DEST}
        }}
    },
    {
        Instruction::RSHIFT, {{
            {TokenCategory::REGISTER, OperandLocation::OP1},
            {TokenCategory::REGISTER, OperandLocation::OP2},
            {TokenCategory::REGISTER, OperandLocation::DEST}
        }}
    },
    {
        Instruction::OUTPUT, {{
            {TokenCategory::CONSTANT, OperandLocation::OP1},
            {TokenCategory::NOTHING, OperandLocation::NONE},
            {TokenCategory::NOTHING, OperandLocation::NONE}
        }}
    },
    {
        Instruction::NOP, {{
            {TokenCategory::NOTHING, OperandLocation::NONE},
            {TokenCategory::NOTHING, OperandLocation::NONE},
            {TokenCategory::NOTHING, OperandLocation::NONE}
        }}
    }
};

std::vector<InstructionIR> parseTokens(const std::vector<Token>& tokens);
InstructionIR parseOperation(const std::vector<Token>& tokens, int& index);
void checkForComma(const Token& token);
void checkForArrow(const Token& token);
InstructionIROperand getAndCheckNextToken(const Token& token, TokenCategory t);
Instruction getInstruction(const Token& token);