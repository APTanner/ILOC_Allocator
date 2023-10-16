#include <algorithm>

#include "allocator.h"
#include "scanner.h"

std::vector<InstructionIR> allocateRegisters(const std::vector<InstructionIR>& instructions, int physicalRegisterCount) {
    int maxRegister{calculateMaxSR(instructions)};
    int n = instructions.size();
    std::vector<InstructionIR> i_instructions(n);

    VRAssignmentTable VRTable(maxRegister+1);
    for (int i = n-1; i >= 0; --i) {
        i_instructions[i] = setVrs(VRTable, instructions[i], i);
    }
    VRTable.checkDefinitions();

    OptimizationTable OptTable(VRTable);
    for (InstructionIR& instruction : i_instructions) {
        checkOptimizations(OptTable, instruction);
    }

    PRAssignmentTable PRTable(physicalRegisterCount, VRTable);
    std::vector<InsertionEvent> insertionEvents{};
    for (int i = 0; i < n; ++i) {
        std::vector<InsertionEvent> events{setPRs(PRTable, i_instructions[i], i)};
        insertionEvents.insert(insertionEvents.end(), events.begin(), events.end());
    }

    std::vector<InstructionIR> f_instructions{performInsertions(i_instructions, insertionEvents, PRTable.getSpillReg())};

    return f_instructions;
}

// ===================================================
// Calculating Next Use ==============================

int calculateMaxSR(const std::vector<InstructionIR>& instructions) {
    int max{};
    for (const InstructionIR& ir : instructions) {
        if (ir.op1.type == OperandType::REGISTER) {
            max = std::max(max,ir.op1.value);
        }
        if (ir.op2.type == OperandType::REGISTER) {
            max = std::max(max,ir.op2.value);
        }
        if (ir.op2.type == OperandType::REGISTER) {
            max = std::max(max,ir.op2.value);
        }
    }
    return max;
}

InstructionIR setVrs(VRAssignmentTable& table, const InstructionIR& instruction, int instructionIndex) {
    if (instruction.opcode == Instruction::STORE) { table.storeOccured(instructionIndex); }
    InstructionIR fIr(instruction);
    // must do destination first
    fIr.dest = table.updateDest(instruction.dest, instructionIndex);
    // then do the operands
    fIr.op1 = table.updateOP(instruction.op1, instructionIndex);
    fIr.op2 = table.updateOP(instruction.op2, instructionIndex);
    return fIr;
}

// ===================================================
// Optimizing ========================================

void checkOptimizations(OptimizationTable& table, InstructionIR& instruction) {
    checkRemat(table, instruction);
}

void checkRemat(OptimizationTable& table, InstructionIR& instruction) {
    switch (instruction.opcode) {
    case Instruction::LOADI:
        instruction.dest.rematValue = instruction.op1.value;
        table.setVRRematValue(instruction.dest.VR, instruction.dest.rematValue);
        break;
    case Instruction::LOAD:
        // makes sure load instructions are storing the value of the memory location
        //  they are going to 
        instruction.op1.rematValue = table.getVRRematValue(instruction.op1.VR);
        break;
    default:
        break;
    }
}

// ===================================================
// Allocating PRs ====================================

std::vector<InsertionEvent> setPRs(PRAssignmentTable& table, InstructionIR& instruction, int instructionIndex) {
    std::vector<InsertionEvent> events{};

    // assign physical registers
    if (instruction.op1.type == OperandType::REGISTER) {
        int PR{assignPRfromVR(table, events, instruction.op1, instructionIndex, true)};
        instruction.op1.PR = PR;
        table.usePRthisInstruction(PR);

        clearDirtiedVRsFromMemory(table, instruction.op1);
    }
    if (instruction.op2.type == OperandType::REGISTER) {
        instruction.op2.PR = assignPRfromVR(table, events, instruction.op2, instructionIndex, true);

        clearDirtiedVRsFromMemory(table, instruction.op2);
    }
    table.clearPRthisInstruction();

    // update next use or release
    if (instruction.op1.type == OperandType::REGISTER) {
        handlePRHandoff(table, instruction.op1);
    }
    if (instruction.op2.type == OperandType::REGISTER) {
        handlePRHandoff(table, instruction.op2);
    }

    // perform same operation for destination register
    if (instruction.dest.type == OperandType::REGISTER) {
        // assign physical registers
        instruction.dest.PR = assignPRfromVR(table, events, instruction.dest, instructionIndex, false);

        // set rematerialization values
        // this can only occur when the VR is defined, so we don't have
        //    to worry about it changing later
        table.setVRRematValue(instruction.dest.VR, instruction.dest.rematValue);

        // set memLoc
        if (instruction.opcode == Instruction::LOAD && !instruction.dest.nextUseDirty) {
            table.setVRmemLoc(instruction.dest.VR, instruction.op1.rematValue);
            debug("We know VR " + to_string(instruction.dest.VR) + " has a value that came from memLoc " + to_string(instruction.op1.rematValue));
        }

        // update next use or release
        handlePRHandoff(table, instruction.dest);
    }
    return events;
}

void clearDirtiedVRsFromMemory(PRAssignmentTable& table, const InstructionIROperand& op) {
    int VR{op.VR};
    if (table.isVRinMemory(VR) && op.nextUseDirty) {
        table.setVRmemLoc(VR, -1);
        debug("VR " + to_string(VR) + " was dirtied before its next use");
    }
}

int assignPRfromVR(PRAssignmentTable& table, std::vector<InsertionEvent>& events, const InstructionIROperand& op, int instructionIndex, bool restoreValue) {
    int VR{op.VR};

    if (table.isVRValid(VR)) {
        return table.getPRfromVR(VR);
    }

    InsertionEvent event = table.assignPR(VR, op.nextUse);
    addEventIfNeeded(events, event, instructionIndex);

    int PR{table.getPRfromVR(VR)};

    if (!restoreValue) {
        return PR;
    }

    event = InsertionEvent{};
    if (table.isVRRematable(VR)) {
        debug("Rematerializing " + to_string(table.getRematValuefromVR(VR)) + " into VR " + to_string(op.VR));
        event.type = InsertionEventType::REMATERIALIZE;
        event.reg = PR;
        event.value = table.getRematValuefromVR(VR);
    }
    else if (table.isVRinMemory(VR)) {
        debug("Restoring from memLoc " + to_string(table.getMemLocfromVR(VR)));
        event.type = InsertionEventType::CLEAN;
        event.reg = PR;
        event.value = table.getMemLocfromVR(VR);
    }
    else {
        debug("Value not restored");
    }
    addEventIfNeeded(events, event, instructionIndex);
    return PR;
}

void handlePRHandoff(PRAssignmentTable& table, const InstructionIROperand& op) {
    if (op.nextUse == -1) {
        table.releasePR(op.VR);
    }
    else {
        table.updateNU(op.PR, op.nextUse);
    }
}

void addEventIfNeeded(std::vector<InsertionEvent>& events, InsertionEvent& newEvent, int instructionIndex)
{
    if (newEvent.type != InsertionEventType::NOTHING) {
        newEvent.instructionIndex = instructionIndex;
        events.push_back(newEvent);
    }
}

std::vector<InstructionIR> performInsertions(const std::vector<InstructionIR>& instructions, const std::vector<InsertionEvent>& insertionEvents, const int spillReg) {
    int index{};
    auto eventsIT = insertionEvents.begin();

    std::vector<InstructionIR> f_instructions{};
    // using the built-in insertion function would be O(n^2)
    while (eventsIT != insertionEvents.end()) {
        if (index < eventsIT->instructionIndex) {
            f_instructions.push_back(instructions[index++]);
        }
        else {
            std::vector<InstructionIR> insertedCode {generateInsertionCode(*eventsIT, spillReg)};
            f_instructions.insert(
                f_instructions.end(),
                insertedCode.begin(),
                insertedCode.end()
            );
            ++eventsIT;
        }
    }
    for (index; index < instructions.size(); ++index) {
        f_instructions.push_back(instructions[index]);
    }
    return f_instructions;
}

std::vector<InstructionIR> generateInsertionCode(const InsertionEvent& insertion, const int spillReg) {
    std::vector<InstructionIR> instructions{};
    InstructionIR instruction{};
    switch(insertion.type) {
    case InsertionEventType::CLEAN:
        instruction.opcode = Instruction::LOADI;
        instruction.op1 = InstructionIROperand{OperandType::CONSTANT, insertion.value, InstructionIROperand::ConstType::Value};
        instruction.dest = InstructionIROperand{OperandType::REGISTER, spillReg, InstructionIROperand::ConstType::PR};
        instructions.push_back(instruction);

        instruction.opcode = Instruction::LOAD;
        instruction.op1 = InstructionIROperand{OperandType::REGISTER, spillReg, InstructionIROperand::ConstType::PR};
        instruction.dest = InstructionIROperand{OperandType::REGISTER, insertion.reg, InstructionIROperand::ConstType::PR};
        instructions.push_back(instruction);
        break;
    case InsertionEventType::SPILL:
        instruction.opcode = Instruction::LOADI;
        instruction.op1 = InstructionIROperand{OperandType::CONSTANT, insertion.value, InstructionIROperand::ConstType::Value};
        instruction.dest = InstructionIROperand{OperandType::REGISTER, spillReg, InstructionIROperand::ConstType::PR};
        instructions.push_back(instruction);

        instruction.opcode = Instruction::STORE;
        instruction.op1 = InstructionIROperand{OperandType::REGISTER, insertion.reg, InstructionIROperand::ConstType::PR};
        instruction.op2 = InstructionIROperand{OperandType::REGISTER, spillReg, InstructionIROperand::ConstType::PR};
        instructions.push_back(instruction);
        break;
    case InsertionEventType::REMATERIALIZE:
        instruction.opcode = Instruction::LOADI;
        instruction.op1 = InstructionIROperand{OperandType::CONSTANT, insertion.value, InstructionIROperand::ConstType::Value};
        instruction.dest = InstructionIROperand{OperandType::REGISTER, insertion.reg, InstructionIROperand::ConstType::PR};
        instructions.push_back(instruction);
        break;
    default:
        error("ERROR::ALLOCATOR: Code insertion with an invalid type detected");
    }
    return instructions;
}
