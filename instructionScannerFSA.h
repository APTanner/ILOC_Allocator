#pragma once
#include <string>
#include <string_view>
#include <fstream>

#include "scanner.h"

void badCharError(std::string_view s, char c);
void noWhitespaceError(std::string_view s);
void checkNextChars(std::ifstream& input, std::string_view chars, std::string& s);
void checkForFollowingBlanks(std::ifstream& input, std::string_view s);
void checkForFollowingWhitespace(std::ifstream& input, std::string_view s);

void s1(std::ifstream& input, Token& token);
void s2(std::ifstream& input, Token& token, std::string& s);
void s6(std::ifstream& input, Token& token, std::string& s);
void s8(std::ifstream& input, Token& token);
void s9(std::ifstream& input, Token& token, std::string& s);
void s11(std::ifstream& input, Token& token, std::string& s);
void s13(std::ifstream& input, Token& token);
void s14(std::ifstream& input, Token& token, std::string& s);
void s19(std::ifstream& input, Token& token);
void s22(std::ifstream& input, Token& token);
void s25(std::ifstream& input, Token& token);
void s28(std::ifstream& input, Token& token);
