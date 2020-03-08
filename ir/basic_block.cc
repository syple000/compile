#include "basic_block.h"

BasicBlock* BlockGraph::Find(const std::vector<BasicBlock*>& vec, BasicBlock* basicBlock) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == basicBlock) {
            return vec[i];
        }
    }
    return nullptr;
}

BlockGraph::BlockGraph() {
    this->_entry = new BasicBlock();
    this->_exit = new BasicBlock();
    AddSuccBlock(this->_entry, this->_exit);
    this->_cur = this->_entry;
    this->_blockVec.push_back(this->_entry);
    this->_blockVec.push_back(this->_exit);
}

bool BlockGraph::AddSuccBlock(BasicBlock* block, BasicBlock* succBlock) {
    if (Find(block->_succ, succBlock) != nullptr) {
        return false;
    } else {
        block->_succ.push_back(succBlock);
        succBlock->_pred.push_back(block);
        return true;
    }
}

bool BlockGraph::AddPredBlock(BasicBlock* block, BasicBlock* prodBlock) {
    if (Find(block->_pred, prodBlock) != nullptr) {
        return false;
    } else {
        block->_pred.push_back(prodBlock);
        prodBlock->_succ.push_back(block);
        return true;
    }
}

void BlockGraph::PushBlock(BasicBlock* basicBlock) {
    this->_blockVec.push_back(basicBlock);
    auto firstInstr = basicBlock->_instrList->LocFirst();
    if (firstInstr != nullptr) {
        this->_instrBlockMap.insert(std::pair<Instruction*, BasicBlock*>(firstInstr, basicBlock));
    }
}

BasicBlock* BlockGraph::FindByFirstInstr(Instruction* instr) {
    auto instrBlockPair = this->_instrBlockMap.find(instr);
    if (instrBlockPair == this->_instrBlockMap.end()) {
        return nullptr;
    } else {
        return instrBlockPair->second;
    }
}

void BlockGraph::InitBlockState(int state) {
    for (int i = 0; i < this->_blockVec.size(); i++) {
        this->_blockVec[i]->_state = state;
    }
}