#include <vector>
#include <optional>
#include <iomanip>

#include "parser.h"
#include "scanner.h"
#include "compilerUtils.h"

std::vector<InstructionIR> parseTokens(const std::vector<Token>& tokens) {
    int index{0};
    int n{static_cast<int>(tokens.size())};
    std::vector<InstructionIR> instructions {};
    while (index < n) {
        InstructionIR ir {parseOperation(tokens, index)};
        instructions.push_back(ir);
    }
    return instructions;
}

InstructionIR parseOperation(const std::vector<Token>& tokens, int& index) {
    if (index == tokens.size())
        error("PARSE_ERROR: Expected INST but got nothing");
    Instruction opcode{getInstruction(tokens[index++])};
    const auto tokenOrder {InstructionIRMap[opcode]};
    InstructionIR iIR{};
    iIR.opcode = opcode;
    for (int j = 0; j < tokenOrder.size(); ++j) {
        TokenCategory nextToken = tokenOrder[j].first;
        //we will know that from now on the token isn't NOTHING
        if (nextToken == TokenCategory::NOTHING)
            continue;
        //check for Comma between index 0 and 1 
        if (j==1) {
            if (index == tokens.size())
            error("PARSE_ERROR: Expected COMMA but got nothing");
            checkForComma(tokens[index++]);
        }
        //check for arrow before last operand
        if (j==2) {
            if (index == tokens.size())
                error("PARSE_ERROR: Expected ARROW but got nothing");
            checkForArrow(tokens[index++]);
        }
        if (index == tokens.size()) 
            error("PARSE_ERROR: Expected operand but got nothing");
        InstructionIROperand operand {getAndCheckNextToken(tokens[index++], nextToken)};
        switch (tokenOrder[j].second)
        {
        case OperandLocation::OP1:
            iIR.op1 = operand;
            break;
        case OperandLocation::OP2:
            iIR.op2 = operand;
            break;
        case OperandLocation::DEST:
            iIR.dest = operand;
            break;
        case OperandLocation::NONE:
            break;
        }
    }
    return iIR;
}

void checkForComma(const Token& token) {
    if (token.category != TokenCategory::COMMA) {
        error("PARSE_ERROR: Expected token type COMMA but got "+token.getCatagoryString());
    }
    return;
}

void checkForArrow(const Token& token) {
    if (token.category != TokenCategory::ARROW) {
        error("PARSE_ERROR: Expected token type ARROW but got "+token.getCatagoryString());
    }
    return;
}


InstructionIROperand getAndCheckNextToken(const Token& token, TokenCategory t) {
    if (token.category != t) {
        // creating temp token to print out its category
        Token temp{};
        temp.category = t;
        error("PARSE_ERROR: Expected token " + temp.getCatagoryString() + " but got token "+token.getCatagoryString());
    }
    if (token.value == -1) 
	{
		error("SCAN_ERROR::INTERNAL: Token of type "+token.getCatagoryString()+" was not assigned a value");
	}
    InstructionIROperand operand{};
    operand.type = (OperandType)t;
    operand.value = token.value;
    return operand;
}

Instruction getInstruction(const Token& token) {
    if (token.category != TokenCategory::INSTRUCTION) {
        error("PARSE_ERROR: Expected INST; got "+token.getCatagoryString());
    }
    return static_cast<Instruction>(token.value);
}