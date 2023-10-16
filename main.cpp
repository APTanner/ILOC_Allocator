#include <iostream>
#include <fstream>
#include <stdlib.h> 
#include <getopt.h>
#include <iomanip>

#include <stdlib.h> 
#include <getopt.h>

#include "scanner.h"
#include "parser.h"
#include "allocator.h"
#include "compilerUtils.h"

using namespace std;

constexpr int DEFAULT_REGISTER_COUNT = 4;

struct options
{
	bool allocate = false;
	bool pretty_print = false;
	bool table_print = false;
	bool token_print = false;
	int registerCount = DEFAULT_REGISTER_COUNT;
	string filename = "";
};

const struct option longopts[] = {
  {"alloc", no_argument, NULL, 'a'},
  {"lexer", no_argument, NULL, 'l'},
  {"pretty-print", no_argument, NULL, 'p'},
  {"table-print", no_argument, NULL, 't'},
  {"help", no_argument, NULL, 'h'},
  {"debug", no_argument, NULL, 'd'}
};

void usage(char* exe_name)
{
	cerr << "usage: " << exe_name << " [-lpthd] file" << endl;
	cerr << "\t-a, --alloc			run the allocator algorithm and pretty-print the results" << endl;
	cerr << "\t-k NUM				use NUM registers when allocating the code" << endl;
	cerr << "\t-l, --lexer			print a list of tokens, one per line" << endl;
	cerr << "\t-p, --pretty-print		print legal ILOC code" << endl;
	cerr << "\t-t, --table-print		print the intermediate representation in tabular form" << endl;
	cerr << "\t-h, --help			print this menu" << endl;
	cerr << "\t-d, --debug			print debug information" << endl;
}

void execute(char* exe_name, options& opts)
{
	ifstream inputFile;

	inputFile.open(opts.filename);
	if (!inputFile)
	{
		cerr << "Invalid filename: " << opts.filename << "." << endl;
		usage(exe_name);
		exit(EXIT_FAILURE);
	}

	vector<Token> tokens = scanFile(inputFile);
	debug("Scanned File; Number of tokens: " + to_string(tokens.size()));
	vector<InstructionIR> instructions = parseTokens(tokens);
	debug("Parsed File; Number of instructions: " + to_string(instructions.size()));

	if (opts.registerCount < 3) {
		error("Bad argument for -k: Cannot perform allocation with less than 3 registers ");
	}

	vector<InstructionIR> f_instructions = allocateRegisters(instructions, opts.registerCount);
	debug("Allocated registers; Number of inserted instructions: " + to_string(f_instructions.size() - instructions.size()));
	
	
	if (opts.pretty_print)
	{
		debug("what?");
		for (InstructionIR& instruction : instructions) 
		{
			instruction.print(PrintType::SCRIPT_REGISTER);
		}
	}
	if (opts.token_print)
	{
		for (Token& token : tokens)
		{
			token.print();
		}
	}
	if (opts.table_print)
	{
		printIRTable(instructions);
	}
	if (opts.allocate)
	{
		for (InstructionIR& instruction : f_instructions) {
			instruction.print(PrintType::PHYSICAL_REGISTER);
		}
	}
}

int main(int argc, char** argv)
{

	char* exe_name = argv[0];
	int ch;

	options opts;

	debug("Entering options.");
	while ((ch = getopt_long(argc, argv, "alpthdk:", longopts, NULL)) != -1)
	{
		debug("Processing option " + string(1, (char)ch));
		switch (ch)
		{
		case 'h':
			usage(exe_name);
			exit(EXIT_SUCCESS);
			break;
		case 'd':
			debugLevel = 1;
			break;
		case 'a':
			opts.allocate = true;
			break;
		case 'k':
			opts.registerCount = atoi(optarg);
			opts.allocate = true;
			break;
		case 'p':
			opts.pretty_print = true;
			break;
		case 't':
			opts.table_print = true;
			break;
		case 'l':
			opts.token_print = true;
			break;
		default:
			// unknown opt
			cerr << "Unknown opt: " << ch << endl;
			usage(exe_name);
			exit(EXIT_FAILURE);
			break;
		}
	}
	debug("Exiting options.");

	if (argc - optind != 1)
	{
		cerr << "Invalid command: filename required." << endl;
		usage(exe_name);
		exit(EXIT_FAILURE);
	}
	else
	{
		opts.filename = string{ argv[optind] };
	}
	debug("Found filename " + opts.filename);

	if (argc == 2) {
		opts.allocate = true;
	}

	execute(exe_name, opts);
}
