#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "parser.h"


using namespace std;

extern int debugLevel;

void error(string msg);
void assertCondition(bool condition, string errmsg);
void debug(string str, int level=1);


//assumes that the first '/' of a comment has already been read.
void ignoreComment(ifstream &input);
bool isSpace(char c); 
void skipBlanks(ifstream &input);
bool skipWhitespace(ifstream &input);
void printTable(vector<int> &table);
void printSummary(vector<int> &summary, int empty=-1, string name = "");
bool isblankspace(char c);
void printIRTable(vector<InstructionIR>& instructions);
