#include "instr.h"

#ifndef BASIC_BLOCK
#define BASIC_BLOCK 1

struct BasicBlock {
    // 节点来源于原instruction list(保持原数据结构，增加新instruction时不应破坏原结构)
    Instruction* _head, *_tail;

    BasicBlock() : _head(nullptr), _tail(nullptr) {}

    BasicBlock(Instruction* head, Instruction* tail) : _head(head), _tail(tail) {}

    void InsertInstr(std::vector<std::string>&& components) {
    }

    // 新指令
    void InsertInstrAfter(Instruction* pre, std::vector<std::string>&& components) {
    }

    void InsertInstr(Instruction* instr) {
        InsertInstrAfter(this->_tail, instr);
    }

    // 插入已存在的指令
    void InsertInstrAfter(Instruction* pre, Instruction* instr) {}

};

#endif