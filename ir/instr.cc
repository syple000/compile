#include "instr.h"

void InstrList::LinkInstrs(Instruction* instr1, Instruction* instr2) {
    if (instr1 != nullptr) {
        instr1->_next = instr2;
    }
    if (instr2 != nullptr) {
        instr2->_pre = instr1;
    }
}

void InstrList::DestroyInstr(Instruction* instr) {
    delete instr;
}

InstrList::InstrList() {
    this->_head = new Instruction();
    this->_tail = new Instruction();
    this->_cur = this->_head;
    LinkInstrs(this->_head, this->_tail);
    this->_size = 0;
}

InstrList::~InstrList() {
    ForwardTraverse(DestroyInstr);
    delete this->_head;
    delete this->_tail;
}

Instruction* InstrList::LocFirst() {
    this->_cur = this->_head->_next;
    return this->_cur;
}

Instruction* InstrList::LocLast() {
    this->_cur = this->_tail->_pre;
    return this->_cur;
}

void InstrList::SetCurInstr(Instruction* instr) {
    this->_cur = instr;
}

Instruction* InstrList::GetCurInstr() {
    return this->_cur == this->_head || this->_cur == this->_tail ? nullptr : this->_cur;
}

Instruction* InstrList::GoAhead() {
    if (this->_cur == this->_tail) {
        return nullptr;
    } else {
        this->_cur = this->_cur->_next;
        return GetCurInstr();
    }
}

Instruction* InstrList::GoBack() {
    if (this->_cur == this->_head) {
        return nullptr;
    } else {
        this->_cur = this->_cur->_pre;
        return GetCurInstr();
    }
}

void InstrList::InsertInstrAfter(Instruction* pre, Instruction* instr) {
    InsertInstrsAfter(pre, instr, instr, 1);
}

void InstrList::InsertInstr(Instruction* instr) {
    InsertInstrAfter(this->_tail->_pre, instr);
}

void InstrList::InsertInstrsAfter(Instruction* pre, Instruction* start, Instruction* end, int size) {
    auto next = pre->_next;
    start->_pre = pre;
    end->_next = next;
    pre->_next = start;
    next->_pre = end;
    this->_size += size;
}

void InstrList::InsertInstrs(Instruction* start, Instruction* end, int size) {
    InsertInstrsAfter(this->_tail->_pre, start, end, size);
}

// 参数中的instrList会被清空
void InstrList::MergeInstrListAfter(Instruction* pre, InstrList* instrList) {
    if (instrList->_size == 0) {
        return;
    } 
    auto start = instrList->_head->_next, end = instrList->_tail->_pre;
    LinkInstrs(instrList->_head, instrList->_tail);
    InsertInstrsAfter(pre, start, end, instrList->_size);
}

void InstrList::MergeInstrList(InstrList* instrList) {
    MergeInstrListAfter(this->_tail->_pre, instrList);
}

Instruction* InstrList::RemoveInstr(Instruction* instr) {
    if (instr == this->_cur) {
        GoAhead();
    }
    LinkInstrs(instr->_pre, instr->_next);
    this->_size--;
    return GetCurInstr();
}

Instruction* InstrList::RemoveInstrs(Instruction* start, Instruction* end) {
    auto cur = start;       
    bool removeCur = cur == this->_cur; 
    int cnt = 1;
    while (cur != end) {
        cur = cur->_next;
        removeCur = removeCur || cur == this->_cur;
        cnt++;
    }
    if (removeCur) {
        this->_cur = end->_next;
    }
    this->_size -= cnt;
    LinkInstrs(start->_pre, end->_next);
    return GetCurInstr();
}

void InstrList::ForwardTraverse(void(*func)(Instruction*)) {
    auto instr = this->_head->_next;
    while (instr != this->_tail) {
        auto next = instr->_next;
        func(instr);
        instr = next;
    }
}

void InstrList::ReverseTraverse(void(*func)(Instruction*)) {
    auto instr = this->_tail->_pre;
    while (instr != this->_head) {
        auto pre = instr->_pre;
        func(instr);
        instr = pre;
    }
}

int InstrList::GetSize() {
    return this->_size;
}