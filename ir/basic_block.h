#include <unordered_map>

#include "instr.h"

#ifndef BASIC_BLOCK
#define BASIC_BLOCK 1

struct BasicBlock {
    InstrList *_instrList;
    // 一般流图中前后驱较少，vec性能较优
    std::vector<BasicBlock*> _pred, _succ;
    int _state;

    BasicBlock() : _instrList(nullptr), _state(-1) {}

    BasicBlock(InstrList* instrList) : _instrList(instrList), _state(-1) {}

    virtual ~BasicBlock() {
        delete this->_instrList;
    }
};

class BlockGraph {
private:
    BasicBlock* _entry, *_exit, *_cur;
    std::vector<BasicBlock*> _blockVec;
    std::unordered_map<Instruction*, BasicBlock*> _instrBlockMap;

    BasicBlock* Find(const std::vector<BasicBlock*>& vec, BasicBlock* basicBlock);

public:
    
    BlockGraph();

    bool AddSuccBlock(BasicBlock* block, BasicBlock* succBlock);

    bool AddPredBlock(BasicBlock* block, BasicBlock* prodBlock);

    void PushBlock(BasicBlock* basicBlock);

    BasicBlock* FindByFirstInstr(Instruction* instr);

    void InitBlockState(int state);
};

#endif