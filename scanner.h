#pragma once

#include <optional>
#include <variant>
#include <vector>
#include <string>

//assinging values to insure comparison with OperandType
enum class TokenCategory
{
	CONSTANT = 0,
	REGISTER = 1,
	INSTRUCTION,
	COMMA,
	ARROW,
	NOTHING
};

enum class Instruction
{
	LOAD,
	LOADI,
	STORE,
	ADD,
	SUB,
	MULT,
	LSHIFT,
	RSHIFT,
	OUTPUT,
	NOP
};

struct Token
{
	TokenCategory category = TokenCategory::NOTHING;
	int value = -1;
	std::string getCatagoryString() const;
	
	void print() const;
};

std::string getInstructionString(Instruction instruction);
std::vector<Token> scanFile(std::ifstream& input);
Token readToken(std::ifstream& input, bool newLine, bool& isReading);
void scanToken(std::ifstream& input, Token& token);
bool scanArrow(std::ifstream& input, Token& token);
bool scanComma(std::ifstream& input, Token& token);
bool scanConstant(std::ifstream& input, Token& token);
bool scanRegister(std::ifstream& input, Token& token);
bool scanInstruction(std::ifstream& input, Token& token);
bool isNum(char c);
int getNum(std::ifstream& input);

