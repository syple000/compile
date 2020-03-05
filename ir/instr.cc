#include "instr.h"

void InstrFlow::destroyInstr(Instruction* instr) {
    delete instr;
}

std::string InstrFlow::GetLabel() {
    return "label_" + std::to_string(this->_labelCnt++);
}

InstrFlow::InstrFlow() {
    this->_head = new Instruction(nullptr, {});
    this->_tail = new Instruction(this->_head, {});
}

InstrFlow::~InstrFlow() {
    Traverse(destroyInstr);
}

void InstrFlow::AddNextInstrFillBackInfo(const std::string& listName, BackFillAttr* attr) {
    if (this->_nextInstrFillBack.find(attr) == this->_nextInstrFillBack.end()) {
        this->_nextInstrFillBack.insert(std::pair<BackFillAttr*, std::vector<std::string>>(attr, std::vector<std::string>()));
    }
    this->_nextInstrFillBack.find(attr)->second.push_back(listName);
}

void InstrFlow::InsertInstrAfter(Instruction* pre, std::vector<std::string>&& components) {
    auto newInstr = new Instruction(pre, std::move(components));
    if (this->_nextInstrFillBack.size() != 0 && newInstr == this->_tail->_pre) {
        if (newInstr->_label.size() == 0) {
            AddLabel(newInstr);
        }
        for (auto listNameAttrPair : this->_nextInstrFillBack) {
            for (auto listName : listNameAttrPair.second) {
                listNameAttrPair.first->BackFill(listName, newInstr->_label);
            }
        }
        this->_nextInstrFillBack.clear();
    }
}

void InstrFlow::InsertInstr(std::vector<std::string>&& components) {
    InsertInstrAfter(this->_tail->_pre, std::move(components));
}


void InstrFlow::InsertInstrsAfter(Instruction* pre, Instruction* start, Instruction* end) {
    Instruction::InsertInstrsAfter(pre, start, end);
}

void InstrFlow::InsertInstrs(Instruction* start, Instruction* end) {
    Instruction::InsertInstrsAfter(this->_tail->_pre, start, end);
}

void InstrFlow::Remove(Instruction* instr) {
    auto pre = instr->_pre, next = instr->_next;
    if (pre != nullptr) {
        pre->_next = next;
    } else {
        this->_head = next;
    }

    if (next != nullptr) {
        next->_pre = pre;
    } else {
        this->_tail = pre;
    }

    delete instr;
}

Instruction* InstrFlow::GetFirst() {
    return this->_head->_next == this->_tail ? nullptr : this->_head->_next;
}

Instruction* InstrFlow::GetLast() {
    return this->_tail->_pre == this->_head ? nullptr : this->_tail->_pre;
}

Instruction* InstrFlow::Next(Instruction* instr) {
    return instr->_next;
}

Instruction* InstrFlow::GetInstrByLabel(const std::string& label) {
    auto labelInstrPair = this->_labelInstrMap.find(label);
    if (labelInstrPair == this->_labelInstrMap.end()) {
        return nullptr;
    } else {
        return labelInstrPair->second;
    }
}

void InstrFlow::AddLabel(Instruction* instr) {
    auto label = GetLabel();
    instr->_label = label;
    this->_labelInstrMap.insert(std::pair<std::string, Instruction*>(label, instr));
} 

void InstrFlow::Traverse(void(*func)(Instruction*)) {
    auto instr = this->_head;
    while (instr != nullptr) {
        auto next = instr->_next;
        func(instr);
        instr = next;
    }
}