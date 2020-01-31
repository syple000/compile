
#include "./memory_track.h"

MemoryTracker::MemoryTracker() : _curRefCount(0) {}

int MemoryTracker::AllocaMemory() {
    if (this->_recycledRefs.size() == 0) {
        return this->_curRefCount++;
    } else {
        int refNumber = this->_recycledRefs.top();
        this->_recycledRefs.pop();
        return refNumber;
    }
}

void MemoryTracker::AddRefOfMemory(int refNumber, Variable* var) {
    auto refItr = this->_memRefs.find(refNumber);
    if (refItr == this->_memRefs.end()) {
        this->_memRefs.insert(std::pair<int, std::set<Variable*>>(refNumber, std::set<Variable*>()));
        refItr = this->_memRefs.find(refNumber);
    }
    refItr->second.insert(var);
}

void MemoryTracker::RemoveRefOfMemory(int refNumber, Variable* var) {
    auto refItr = this->_memRefs.find(refNumber);
    refItr->second.erase(var);
    if (refItr->second.size() == 0) {
        this->_memRefs.erase(refItr);
        this->_recycledRefs.push(refNumber);
    }
}