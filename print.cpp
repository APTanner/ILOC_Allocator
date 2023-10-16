#include "print.h"

void PRAssignmentTable::printTable(int instructionIndex) const {
    std::cout << "//Instruction: " << instructionIndex;
    std::cout << '\n' << "//   VR        |";
    for (int i = 0; i < VRtoPR.size(); ++i) {
        if (VRtoPR[i] != -1) {
            std::cout << setw(3) << i << '|';
        }
    }
    std::cout << '\n' << "//   VRtoPR    |";
    for (int i = 0; i < VRtoPR.size(); ++i) {
        if (VRtoPR[i] != -1) {
            std::cout << setw(3) << VRtoPR[i] << '|';
        }
    }
    std::cout << '\n' << "//   nextUse   |";
    for (int i = 0; i < VRtoPR.size(); ++i) {
        if (VRtoPR[i] != -1) {
            std::cout << setw(3) << PRtoNU[VRtoPR[i]] << '|';
        }
    }
    std::cout << '\n' << "//   PRtoVR   |";
    for (int i = 0; i < PRtoVR.size(); ++i) {
            std::cout << setw(3) << "PR: " << i << '=' << PRtoVR[i] << '|';
    }
    std::cout << '\n' << "//   memLoc(VR)|";
    for (int i = 0; i < memLoc.size(); ++i) {
        if (memLoc[i] != -1) {
            std::cout << setw(3) << i << '=' << memLoc[i] << '|';
        }
    }
    std::cout << '\n';
}


void VRAssignmentTable::printTable(int instructionIndex) {
    std::cout << "//Instruction: " << instructionIndex << '\n';
    std::cout << "//   SR    |";
    for (int i = 0; i < SRtoVR.size(); ++i) {
        std::cout << setw(3) << i << '|';
    }
    std::cout << '\n' << "// SRtoVR  |";
    for (const auto& elem : SRtoVR) {
        std::cout << setw(3) << elem << '|';
    }
    std::cout << '\n' << "// SRtoLU  |";
    for (const auto& elem : SRtoLU) {
        std::cout << setw(3) << elem << '|';
    }
    std::cout << '\n' << "// SRtoNU  |";
    for (const auto& elem : SRtoNU) {
        std::cout << setw(3) << elem << '|';
    }
    std::cout << '\n';
}

void InstructionIROperand::debugPrint() const {
    switch (type)
    {
    case OperandType::CONSTANT:
        std::cout << '|' << setw(7) << "CONST" << '|';
        std::cout << setw(7) << value;
        std::cout << '|';
        std::cout << setw(5) << ' ';
        std::cout << '|';
        std::cout << setw(5) << ' ';
        std::cout << '|';
        std::cout << setw(5) << ' ';
        std::cout << '|';
        break;
    case OperandType::REGISTER:
        std::cout << '|' <<  setw(7) << " REG " << '|';
        std::cout << setw(7) << ' ';
        std::cout << '|';
        std::cout << setw(5) << value;
        std::cout << '|';
        std::cout << setw(5) << VR;
        std::cout << '|';
        std::cout << setw(5) << PR;
        std::cout << '|';
        break;
    case OperandType::NOTHING:
        std::cout << '|' <<  setw(7) << " NONE " << '|';
        std::cout << setw(7) << ' ';
        std::cout << '|';
        std::cout << setw(5) << ' ';
        std::cout << '|';
        std::cout << setw(5) << ' ';
        std::cout << '|';
        std::cout << setw(5) << ' ';
        std::cout << '|';
        break;
    default:
        break;
    }
}

std::string InstructionIROperand::getOperandString(const PrintType pType) const {
    switch (type)
    {
    case OperandType::CONSTANT:
        return to_string(value);
    case OperandType::REGISTER: {
            std::string s{"r"};
            switch (pType)
            {
            case PrintType::SCRIPT_REGISTER:
                s += to_string(value);
                    break;
            case PrintType::VIRTUAL_REGISTER:
                s += to_string(VR);
                break;
            case PrintType::PHYSICAL_REGISTER:
                s += to_string(PR);
                break;
            }
            return s;
        }
    case OperandType::NOTHING:
        return "-";
    default:
        break;
    }
    return "";
}

void InstructionIR::print(const PrintType pType) const {
    std::cout << getInstructionString(opcode);
    const auto operandOrder{InstructionIRMap[opcode]};
    for (int i=0; i < operandOrder.size(); ++i) {
        OperandLocation oploc = operandOrder[i].second;
        if (oploc == OperandLocation::NONE)
            continue;
        if (i==1)
            std::cout << ',';
        if (i==2)
            std::cout << " =>";
        switch (oploc) {
        case OperandLocation::OP1:
            std::cout << ' ' << op1.getOperandString(pType);
            break;
        case OperandLocation::OP2:    
            std::cout << ' ' << op2.getOperandString(pType);
            break;
        case OperandLocation::DEST:
            std::cout << ' ' << dest.getOperandString(pType);
            break;
        }
    }
    std::cout << std::endl;
    return;
}

void InstructionIR::debugPrint(int instructionIndex) const {
    std::cout << "//" << setw(3) << instructionIndex << '|';
    std::cout << setw(6) << getInstructionString(opcode);
    op1.debugPrint();
    op2.debugPrint();
    dest.debugPrint();
    std::cout << std::endl;
}

void InstructionIR::tablePrint() const {
    PrintType p{PrintType::SCRIPT_REGISTER};
    std::cout << std::left << setw(8) << " "+getInstructionString(opcode) <<
        "||" << setw(5) << " "+op1.getOperandString(p) << '|' << setw(5) << " "+op2.getOperandString(p) << 
        '|' << setw(5) << " "+dest.getOperandString(p) << '|' << endl;
}