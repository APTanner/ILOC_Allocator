#pragma once

#include <vector>
#include <stack>
#include <climits>

#include "parser.h"

enum class InsertionEventType {
    NOTHING,
    SPILL,
    REMATERIALIZE,
    CLEAN
};

struct InsertionEvent {
    int instructionIndex{-1};
    InsertionEventType type{InsertionEventType::NOTHING};
    int reg{-1};
    int value{-1};
};

class VRAssignmentTable {
private:
    std::vector<int> SRtoVR;
    std::vector<int> SRtoLU;
    std::vector<int> SRtoNU;
    int numVRs;
    int currentLive;
    int maxLive;
    int nextStoreInstruction;
public:
    VRAssignmentTable(int maxRegister) 
    :   SRtoVR(maxRegister + 1, -1), 
        SRtoLU(maxRegister + 1, -1), 
        SRtoNU(maxRegister + 1, -1), 
        numVRs(0),
        currentLive(0),
        maxLive(0),
        nextStoreInstruction(INT_MAX)
        {};

    InstructionIROperand updateOP(const InstructionIROperand& op, int instIndex);
    InstructionIROperand updateDest(const InstructionIROperand& op, int instIndex);
    void resetSR(int SR);
    void incrementCurrentLive();
    void decrementCurrentLive();

    void storeOccured(int instructionIndex);

    int getMaxLive() const;
    int getNumVRs() const;
    void checkDefinitions() const;

    void printTable(int instructionIndex);
};

class OptimizationTable {
private:
    std::vector<int> rematValues;
public:
    OptimizationTable(VRAssignmentTable table)
        : rematValues(table.getNumVRs(), -1)
        {};

    void setVRRematValue(int VR, int value);
    int getVRRematValue(int VR) const;
};

class PRAssignmentTable {
private:
    bool isSpilling;
    int spillReg;
    int spillMemLoc;
    int registerCount;

    int usedPRthisInstruction;

    std::stack<int> PRs;

    std::vector<int> VRtoPR;
    std::vector<int> memLoc;
    std::vector<int> VRtoRemat;

    std::vector<int> PRtoVR;
    std::vector<int> PRtoNU;
public:
    PRAssignmentTable(int registerCount, const VRAssignmentTable& vrTable) {
        int maxLiveRegisters{vrTable.getMaxLive()};
        PRs = std::stack<int>();
        if (maxLiveRegisters > registerCount) {
            isSpilling = true;
            spillReg = registerCount-1;
            for (int i = registerCount-2; i >= 0; --i) { PRs.push(i); }
            PRtoVR = std::vector<int>(registerCount-1, -1);
            PRtoNU = std::vector<int>(registerCount-1, -1);
        }
        else {
            isSpilling = false;
            spillReg = -1;
            for (int i = registerCount-1; i >= 0; --i) { PRs.push(i); }
            PRtoVR = std::vector<int>(registerCount, -1);
            PRtoNU = std::vector<int>(registerCount, -1);
        }
        usedPRthisInstruction = -1;
        spillMemLoc = 32768;
        VRtoPR = std::vector<int>(vrTable.getNumVRs(), -1);
        memLoc = std::vector<int>(vrTable.getNumVRs(), -1);
        VRtoRemat = std::vector<int>(vrTable.getNumVRs(), -1);
    }
    int getSpillReg() const;
    int getNextSpillMemLoc();
    int getPRfromVR(int VR) const;
    int getMemLocfromVR(int VR) const;
    int getRematValuefromVR(int VR) const;
    bool getReplacementPR(int& PR);
    
    InsertionEvent assignPR(int VR, int nextUse);

    void releasePR(int VR);
    void setVRRematValue(int VR, int value);
    void setVRmemLoc(int VR, int memoryLocation);
    void updateNU(int PR, int nextUse);
    void usePRthisInstruction(int PR);
    void clearPRthisInstruction();

    bool isVRValid(int VR) const;
    bool isVRinMemory(int VR) const;
    bool isVRRematable(int VR) const;

    void printTable(int instructionIndex) const;
};