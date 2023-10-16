#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <variant>

#include "instructionScannerFSA.h"
#include "scanner.h"
#include "compilerUtils.h"

void badCharError(std::string_view s, char c)
{
	error("SCAN_ERROR::INSTRUCTION: Unexpected character " + to_string(c) + " after partial token " + std::string(s));
}

void noWhitespaceError(std::string_view s)
{
	error("SCAN_ERROR::INSTRUCTION: No following whitespace behind operation " + std::string(s));
}

void checkNextChars(std::ifstream& input, std::string_view chars, std::string& s)
{
	char _c;
	for (const char c : chars)
	{
		input.get(_c);
		if (_c != c)
			badCharError(s, _c);
		s += c;
	}
	return;
}

void checkForFollowingBlanks(std::ifstream& input, std::string_view s)
{
	if (!isblankspace(input.peek()))
		noWhitespaceError(s);
	return;
}

void checkForFollowingWhitespace(std::ifstream& input, std::string_view s)
{
	if (!isspace(input.peek()))
		noWhitespaceError(s);
	return;
}

void s1(std::ifstream& input, Token& token)
{
	char c;
	input.get(c);
	std::string s{ "s" };
	switch (c)
	{
	case 't':
		s+='t';
		s2(input, token, s);
		break;
	case 'u':
		s+='u';
		s6(input, token, s);
		break;
	default:
		badCharError(s, c);
		break;
	}
	return;
}

void s2(std::ifstream& input, Token& token, std::string& s)
{
	checkNextChars(input, "ore", s);
	checkForFollowingBlanks(input, s);
	token.value = static_cast<int>(Instruction::STORE);
	return;
}

void s6(std::ifstream& input, Token& token, std::string& s)
{
	checkNextChars(input, "b", s);
	checkForFollowingBlanks(input, s);
	token.value = static_cast<int>(Instruction::SUB);
	return;
}

void s8(std::ifstream& input, Token& token)
{
	char c;
	input.get(c);
	std::string s{ "l" };
	switch (c)
	{
	case 'o':
		s+='o';
		s9(input, token, s);
		break;
	case 's':
		s+='s';
		s14(input, token, s);
		break;
	default:
		badCharError(s, c);
		break;
	}
	return;
}

void s9(std::ifstream& input, Token& token, std::string& s)
{
	checkNextChars(input, "ad", s);
	s11(input, token, s);
	return;
}

void s11(std::ifstream& input, Token& token, std::string& s)
{
	if (input.peek() == 'I')
	{
		input.get();
		s += 'I';
	}
	checkForFollowingBlanks(input, s);
	if (s == "load") {
		token.value = static_cast<int>(Instruction::LOAD);
	}
	else {
		token.value = static_cast<int>(Instruction::LOADI);
	}
	return;
}

void s13(std::ifstream& input, Token& token)
{
	std::string s{ "r" };
	checkNextChars(input, "s", s);
	s14(input, token, s);
	return;
}

void s14(std::ifstream& input, Token& token, std::string& s)
{
	checkNextChars(input, "hift", s);
	checkForFollowingBlanks(input, s);
	if (s == "lshift") {
		token.value = static_cast<int>(Instruction::LSHIFT);
	}
	else {
		token.value = static_cast<int>(Instruction::RSHIFT);
	}
	return;
}

void s19(std::ifstream& input, Token& token)
{
	std::string s{ "m" };
	checkNextChars(input, "ult", s);
	checkForFollowingBlanks(input, s);
	token.value = static_cast<int>(Instruction::MULT);
	return;
}

void s22(std::ifstream& input, Token& token)
{
	std::string s{ "a" };
	checkNextChars(input, "dd", s);
	checkForFollowingBlanks(input, s);
	token.value = static_cast<int>(Instruction::ADD);
	return;
}

void s25(std::ifstream& input, Token& token)
{
	std::string s{ "n" };
	checkNextChars(input, "op", s);
	checkForFollowingWhitespace(input, s);
	token.value = static_cast<int>(Instruction::NOP);
	return;
}

void s28(std::ifstream& input, Token& token)
{
	std::string s{ "o" };
	checkNextChars(input, "utput", s);
	checkForFollowingBlanks(input, s);
	token.value = static_cast<int>(Instruction::OUTPUT);
	return;
}
