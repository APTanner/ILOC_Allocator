#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctype.h>

#include "compilerUtils.h"
#include "parser.h"

using namespace std;

int debugLevel = 0; 

void error(string msg) {
  cerr << msg << endl;
  exit(EXIT_FAILURE);
}

void assertCondition(bool condition, string errmsg) {
  if (!condition) {
    cerr << "Assertion failed: " << errmsg << endl;
    exit(EXIT_FAILURE);
  }
}

void debug(string str, int level) {
  if (debugLevel >= level)
    cout << "//" << str << endl;
}


//assumes that the first '/' of a comment has already been read.
void ignoreComment(ifstream &input) {
  char c;
  if(input.good()) {
    input.get(c);
    if(input.peek() == '/') {
      while(input.good() && c!= '\n') {
        input.get(c);
      }
    } else {
      error("invalid comment: ");
    }
  }
}


bool isblankspace(char c) {
  return (c == ' ') || (c == '\t');
}

//spaces and tabs only
void skipBlanks(ifstream &input) {
  char c;
  while(input.good() && isblankspace(input.peek())) {
    input.get(c);
  }
  return;
}

//includes newlines
bool skipWhitespace(ifstream &input) {
  char c{};
  bool newLine{false};
  while(input.good() && isspace(input.peek())) {
    input.get(c);
    if (c == '\n')
      newLine = true;
  }
  return newLine;
}


void printSummary(vector<int> &summary, int empty, string name) {
  cout << "//" << name << "| ";
  int i = 0;
  for(auto elem : summary) {
    if(elem != empty)
      cout << i << " : " << elem << " | ";
    i++;
  }
  cout << " | " << endl;
}

void printIRTable(vector<InstructionIR>& instructions) {
  cout << '|' << setw(5) << "index" << '|' << setw(8) << "opcode " <<
    "||" << setw(5) << "op1 " << '|' << setw(5) << "op2 " << '|' <<
    setw(5) << "dest " << '|' << endl;
  cout << '|' << setw(5) << "" << '|' << setw(8) << "" <<
    "||" << setw(5) << "sr " << '|' << setw(5) << "sr " << '|' <<
    setw(5) << "sr " << '|' << endl;
  int i{};
  for (const InstructionIR& inst : instructions) {
    cout << std::left << '|' << setw(5) << i++ << '|';
    inst.tablePrint();
  }
}
