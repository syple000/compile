#include <queue>
#include <unordered_map>
#include <set>

#include "./variable.h"

class MemoryTracker {
private:
    std::priority_queue<int> _recycledRefs;
    int _curRefCount;
    std::unordered_map<int, std::set<Variable*>> _memRefs;

public:
    MemoryTracker();

    int AllocaMemory();

    void AddRefOfMemory(int refNumber, Variable* var);

    void RemoveRefOfMemory(int refNumber, Variable* var);
    
};