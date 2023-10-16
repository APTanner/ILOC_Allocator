#pragma once

#include <vector>
#include <stack>

#include "allocatorTables.h"
#include "compilerUtils.h"
#include "parser.h"

std::vector<InstructionIR> allocateRegisters(const std::vector<InstructionIR>& instructions, int physicalRegisterCount);
// setting virtual registers
int calculateMaxSR(const std::vector<InstructionIR>& instructions);
InstructionIR setVrs(VRAssignmentTable& table, const InstructionIR& instruction, int instructionIndex);
// optimization pass
void checkOptimizations(OptimizationTable& table, InstructionIR& instruction);
void checkRemat(OptimizationTable& table, InstructionIR& instruction);
// setting physical registers
std::vector<InsertionEvent> setPRs(PRAssignmentTable& table, InstructionIR& instruction, int instructionIndex);
int assignPRfromVR(PRAssignmentTable& table, std::vector<InsertionEvent>& events, const InstructionIROperand& op, int instructionIndex, bool restoreValue);
void handlePRHandoff(PRAssignmentTable& table, const InstructionIROperand& op);
void clearDirtiedVRsFromMemory(PRAssignmentTable& table, const InstructionIROperand& op);
// inserting new operations
void addEventIfNeeded(std::vector<InsertionEvent>& events, InsertionEvent& newEvent, int instructionIndex);
std::vector<InstructionIR> performInsertions(const std::vector<InstructionIR>& instructions, const std::vector<InsertionEvent>& insertionEvents, const int spillReg);
std::vector<InstructionIR> generateInsertionCode(const InsertionEvent& insertion, const int spillReg);



