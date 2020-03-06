#include "instr.h"

#ifndef BASIC_BLOCK
#define BASIC_BLOCK 1

struct BasicBlock {
    InstrList *_instrList;
    std::vector<BasicBlock*> _prod, _succ;

    BasicBlock() : _instrList(nullptr) {}

    BasicBlock(InstrList* instrList) : _instrList(instrList) {}

    virtual ~BasicBlock() {
        delete this->_instrList;
    }
};

class BlockGraph {
private:
};

#endif