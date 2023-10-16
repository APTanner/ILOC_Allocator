#include "allocatorTables.h"
#include "compilerUtils.h"


// ===================================================
// VRAssignment Table ================================


InstructionIROperand VRAssignmentTable::updateOP(const InstructionIROperand& op, int instIndex) {
    InstructionIROperand res(op);
    if (op.type != OperandType::REGISTER) {
        return res;
    }
    int SR{op.value};

    if (SRtoVR[SR] == -1) {
        SRtoVR[SR] = numVRs++;
        incrementCurrentLive();
    }
    res.VR = SRtoVR[SR];
    int nextUse{SRtoLU[SR]};
    res.nextUse = nextUse;
    SRtoNU[SR] = nextUse;
    SRtoLU[SR] = instIndex;

    // if a store doesn't occur before the next use 
    if (nextUse < nextStoreInstruction) {
        res.nextUseDirty = false;
    }

    return res;
}

InstructionIROperand VRAssignmentTable::updateDest(const InstructionIROperand& op, int instIndex) {
    InstructionIROperand res(op);
    if (op.type != OperandType::REGISTER) {
        return res;
    }
    int SR{op.value};
    if (SRtoVR[SR] == -1) {
        SRtoVR[SR] = numVRs++;
        incrementCurrentLive();
    }
    res.VR = SRtoVR[SR];
    int nextUse{SRtoLU[SR]};
    res.nextUse = nextUse;
    SRtoNU[SR] = nextUse;
    SRtoLU[SR] = instIndex;
    decrementCurrentLive();
    resetSR(SR);

    // if a store doesn't occur before the next use 
    if (nextUse < nextStoreInstruction) {
        res.nextUseDirty = false;
    }

    return res;
}

void VRAssignmentTable::incrementCurrentLive() {
    maxLive = std::max(maxLive,++currentLive);
}

void VRAssignmentTable::decrementCurrentLive() {
    --currentLive;
}

void VRAssignmentTable::storeOccured(int instructionIndex) {
    nextStoreInstruction = instructionIndex;
}

int VRAssignmentTable::getMaxLive() const {
    return maxLive;
}

void VRAssignmentTable::resetSR(int SR) {
    SRtoVR[SR] = -1;
    SRtoLU[SR] = -1;
}

int VRAssignmentTable::getNumVRs() const {
    return numVRs;
}

void VRAssignmentTable::checkDefinitions() const {
    for (const auto elem : SRtoLU) {
        if (elem != -1) {
            error("ERROR::ALLOCATION: Register " + to_string(elem) + " was never defined in the original code");
        }
    }
}

// ===================================================
// Optimization Table ================================

void OptimizationTable::setVRRematValue(int VR, int value) {
    rematValues[VR] = value;
}
int OptimizationTable::getVRRematValue(int VR) const {
    return rematValues[VR];
}

// ===================================================
// PRAssignment Table ================================

int PRAssignmentTable::getSpillReg() const {
    return spillReg; 
};

int PRAssignmentTable::getPRfromVR(int VR) const {
    return VRtoPR[VR];
}

int PRAssignmentTable::getMemLocfromVR(int VR) const {
    return memLoc[VR];
}

void PRAssignmentTable::usePRthisInstruction(int PR) {
    usedPRthisInstruction = PR;
}

void PRAssignmentTable::clearPRthisInstruction() {
    usedPRthisInstruction = -1;
}

int PRAssignmentTable::getNextSpillMemLoc() {
    int res{spillMemLoc};
    spillMemLoc += 4;
    return res;
}

int PRAssignmentTable::getRematValuefromVR(int VR) const {
    return VRtoRemat[VR];
}

bool PRAssignmentTable::isVRRematable(int VR) const {
    return VRtoRemat[VR] != -1;
}

InsertionEvent PRAssignmentTable::assignPR(int VR, int nextUse) {
    InsertionEvent spillEvent{};
    if (isSpilling && PRs.empty()) {
        debug("**need to free a PR**");
        int spillPR{};
        bool needToSpill = getReplacementPR(spillPR);
        int spillVR {PRtoVR[spillPR]};
        if (needToSpill) {
            int thisMemLoc = getNextSpillMemLoc();
            memLoc[spillVR] = thisMemLoc;
            // adjust insertion event
            spillEvent.type = InsertionEventType::SPILL;
            spillEvent.reg = spillPR;
            spillEvent.value = thisMemLoc;
            // ---------------------
            debug("spilled VR " + to_string(spillVR) + " to memLoc " + to_string(thisMemLoc));
        }
        releasePR(spillVR);
    }
    int newPR {PRs.top()};
    PRs.pop();
    VRtoPR[VR] = newPR;
    PRtoVR[newPR] = VR;
    PRtoNU[newPR] = nextUse;
    return spillEvent;
}

void PRAssignmentTable::releasePR(int VR) {
    int PR{getPRfromVR(VR)};
    VRtoPR[VR] = -1;
    PRtoVR[PR] = -1;
    PRtoNU[PR] = -1;
    PRs.push(PR);
}

void PRAssignmentTable::updateNU(int PR, int nextUse) {
    PRtoNU[PR] = nextUse;
}

void PRAssignmentTable::setVRRematValue(int VR, int value) {
    VRtoRemat[VR] = value;
} 

void PRAssignmentTable::setVRmemLoc(int VR, int memoryLocation) {
    memLoc[VR] = memoryLocation;
}

bool PRAssignmentTable::isVRValid(int VR) const {
    return VRtoPR[VR] != -1;
}

bool PRAssignmentTable::isVRinMemory(int VR) const {
    return memLoc[VR] != -1;
}

// returns true if a spill is required
bool PRAssignmentTable::getReplacementPR(int& PR) {
    // get rematerializable value
    for (int i = 0; i < PRtoVR.size(); ++i) {
        if (i != usedPRthisInstruction && VRtoRemat[PRtoVR[i]] != -1) {
            PR = i;
            debug("Chose a PR with a rematerializable value");
            return false;
        }
    }

    // get already spilled value
    for (int i = 0; i < PRtoVR.size(); ++i) {
        if (i != usedPRthisInstruction && memLoc[PRtoVR[i]] != -1) {
            PR = i;
            debug("Chose a PR with a value already in memory");
            return false;
        }
    }

    // get value which will be stored in memory the longest
    int longestNU{};
    for (int i = 0; i < PRtoNU.size(); ++i) {
        if (i != usedPRthisInstruction && PRtoNU[i] > longestNU) {
            longestNU = PRtoNU[i];
            PR = i;
        }
    }
    debug("Chose the PR with the farthest next use");
    return true;
}