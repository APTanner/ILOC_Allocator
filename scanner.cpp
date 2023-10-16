#include <iostream>
#include <fstream>
#include <vector>
#include <variant>

#include "scanner.h"
#include "compilerUtils.h"
#include "instructionScannerFSA.h"

void Token::print() const
{
	switch (category)
	{
	case TokenCategory::INSTRUCTION:
		std::cout << "<INST, " << getInstructionString((Instruction)value) << ">" << std::endl;
		break;
	case TokenCategory::REGISTER:
		std::cout << "<REG,  r" << to_string(value) << ">" << std::endl;
		break;
	case TokenCategory::CONSTANT:
		std::cout << "<CONST, " << to_string(value) << ">" << std::endl;
		break;
	case TokenCategory::COMMA:
		std::cout << "<COMMA, ','>" << std::endl;
		break;
	case TokenCategory::ARROW:
		std::cout << "<ARROW, '=>'>" << std::endl;
		break;
	default:
		std::cout << "SCAN_ERROR: Token was not assigned a value" << endl;
		break;
	}
	return;
}

std::string Token::getCatagoryString() const
{
	switch (category)
	{
	case TokenCategory::INSTRUCTION:
		return "INST";
	case TokenCategory::REGISTER:
		return "REG";
	case TokenCategory::CONSTANT:
		return "CONST";
	case TokenCategory::COMMA:
		return "COMMA";
	case TokenCategory::ARROW:
		return "ARROW";
	default:
		error("SCAN_ERROR::INTERNAL: Token was not assigned a valid category");
		break;
	}
	return "";
}
std::string getInstructionString(Instruction inst)
{
	switch (inst)
	{
	case Instruction::LOAD:
		return "load";
	case Instruction::LOADI:
		return "loadI";
	case Instruction::STORE:
		return "store";
	case Instruction::ADD:
		return "add";
	case Instruction::SUB:
		return "sub";
	case Instruction::MULT:
		return "mult";
	case Instruction::LSHIFT:
		return "lshift";
	case Instruction::RSHIFT:
		return "rshift";
	case Instruction::OUTPUT:
		return "output";
	case Instruction::NOP:
		return "nop";
	default:
		return "error";
	}
}

std::vector<Token> scanFile(std::ifstream& input)
{
	bool isReading{ true };
	skipWhitespace(input);
	Token t{ readToken(input, true, isReading) };
	std::vector<Token> tokens{};
	while (isReading)
	{
		tokens.push_back(t);
		bool newLine {skipWhitespace(input)};
		t = readToken(input, newLine, isReading);
	}
	return tokens;
}

Token readToken(std::ifstream& input, bool newLine, bool& isReading)
{
	char c{};
	if (input.good() && input.peek() == '/')
	{
		debug("Ignoring Comment");
		ignoreComment(input); //comment must end with a new line
		skipWhitespace(input);
		return readToken(input, true, isReading);
	}
	else
	{
		Token t;
		if (!input.good())
		{
			isReading = false;
			return t;
		}
		scanToken(input, t);
		if (t.category == TokenCategory::INSTRUCTION && !newLine)
			error("SCAN::ERROR::INSTRUCTION: Expected newline but got token of type INST instead");
		return t;
	}
}

void scanToken(std::ifstream& input, Token& token)
{
	scanArrow(input, token)	   ||
	scanComma(input, token)    ||
	scanConstant(input, token) ||
	scanRegister(input, token) ||
	scanInstruction(input, token);
	return;
}

bool scanArrow(std::ifstream& input, Token& token)
{
	char c;
	if (input.peek() == '=')
	{
		input.get(c);
		input.get(c);
		if (c == '>')
		{
			token.category = TokenCategory::ARROW;
			return true;
		}
		error("SCAN_ERROR::ARROW: Expected character '>' but found " + string(1,c));
	}
	return false;
}

bool scanComma(std::ifstream& input, Token& token)
{
	char c;
	if (input.peek() == ',')
	{
		input.get(c);
		token.category = TokenCategory::COMMA;
		return true;
	}
	return false;
}

bool scanConstant(std::ifstream& input, Token& token)
{
	char c;
	if (isNum(input.peek()))
	{
		int val{ getNum(input) };
		token.category = TokenCategory::CONSTANT;
		token.value = val;
		return true;
	}
	return false;
}

bool scanRegister(std::ifstream& input, Token& token)
{
	char c;
	if (input.peek() == 'r')
	{
		input.get(c);
		if (isNum(input.peek()))
		{
			int val{ getNum(input) };
			token.category = TokenCategory::REGISTER;
			token.value = val;
			return true;
		}
		input.unget();
	}

	return false;
}

bool scanInstruction(std::ifstream& input, Token& token)
{
	char c;
	input.get(c);
	//set the catagory here
	//scanning the instruction must be the last scan
	//therefore, its either an instruction or an error
	token.category = TokenCategory::INSTRUCTION;
	switch (c)
	{
		case 's':
			s1(input, token);
			break;
		case 'l':
			s8(input, token);
			break;
		case 'r':
			s13(input, token);
			break;
		case 'm':
			s19(input, token);
			break;
		case 'a':
			s22(input, token);
			break;
		case 'n':
			s25(input, token);
			break;
		case 'o':
			s28(input, token);
			break;
		default:
			for (int i = 0; i < 100; ++i) {
				std::cout << c;
				input.get(c);
			}
			error("SCAN_ERROR::INSTRUCTION: No instruction begins with character " + string(1,c));
			return false;
	}
	return true;
}

bool isNum(char c)
{
	return '0' <= c && c <= '9';
}

int getNum(std::ifstream& input)
{
	int res{};
	char c{};
	while (input.good() && isNum(input.peek()))
	{
		input.get(c);
		res *= 10;
		res += (c - '0');
	}
	debug("Found number: " + to_string(res));
	return res;
}
