#include "instr.h"
#include <cassert>

void InstrList::DestroyInstr(Instruction* instr) {
    delete instr;
}

InstrList::InstrList() {
    this->_curItr = this->_instrList.begin();
}

InstrList::InstrList(const std::string& filePath) {
    this->_curItr = this->_instrList.begin();

    IO<std::string> io(String2String, String2String);
    Buffer buf(64);
    if (io.ReadFile(buf, filePath) != 0) {
        std::cout << "can not open " << filePath << std::endl;
        return;
    }
    RegExprEngine blankRegExpr("\\s*");
    RegExprEngine colonRegExpr(":*");
    while (true) {
        auto instrStr = buf.GetNextLine();
        if (instrStr.size() == 0) {
            break;
        } else {
            auto instrComps = StringUtil::Split(instrStr, colonRegExpr);
            Instruction* instr;
            // label : components
            if (instrComps.size() == 2) {
                instr = new Instruction(StringUtil::Split(instrComps[1], blankRegExpr));
                instr->_label = instrComps[0];
            } else {

#ifdef DEBUG_CODE
                if (instrComps.size() != 1) {
                    std::cout << "instr: " << instrStr << " format error!" << std::endl;
                }
#endif

                instr = new Instruction(StringUtil::Split(instrComps[0], blankRegExpr));
            }
            InsertInstr(instr);
        }
    }
}

InstrList::~InstrList() {
    ForwardTraverse(DestroyInstr);
}

void InstrList::LocFirst() {
    this->_curItr = this->_instrList.begin();
}

void InstrList::LocLast() {
    this->_curItr = this->_instrList.end();
}

void InstrList::SetCurInstr(const std::string& label) {
    for (auto instrItr = this->_instrList.begin(); instrItr != this->_instrList.end(); instrItr++) {
        if ((*instrItr)->_label == label) {
            this->_curItr = instrItr;
            break;
        }
    }
}

Instruction* InstrList::GetCurInstr() {
    if (this->_curItr == this->_instrList.end()) {
        return nullptr;
    } else {
        return *this->_curItr;
    }
}

Instruction* InstrList::GoAhead() {
    if (this->_curItr == this->_instrList.end() || (++this->_curItr) == this->_instrList.end()) {
        return nullptr;
    }
    return *this->_curItr;
}

Instruction* InstrList::GoBack() {
    if (this->_curItr == this->_instrList.begin()) {
        return nullptr;
    }
    this->_curItr--;
    return *this->_curItr;
}

void InstrList::InsertInstr(Instruction* instr) {
    this->_instrList.insert(this->_curItr, instr);
}

void InstrList::MergeInstrList(InstrList& instrList) {
    this->_instrList.insert(this->_curItr, instrList._instrList.begin(), instrList._instrList.end());
    instrList._instrList.clear();
}

Instruction* InstrList::RemoveInstr() {
    if (this->_curItr == this->_instrList.end()) {
        return nullptr;
    } else {
        auto instr = *this->_curItr;
        this->_curItr = this->_instrList.erase(this->_curItr);
        return instr;
    }
}

void InstrList::ForwardTraverse(void(*func)(Instruction*)) {
    for (auto itr = this->_instrList.begin(); itr != this->_instrList.end(); itr++) {
        func(*itr);
    }
}

void InstrList::ReverseTraverse(void(*func)(Instruction*)) {
    for (auto itr = this->_instrList.rbegin(); itr != this->_instrList.rend(); itr++) {
        func(*itr);
    }
}

int InstrList::GetSize() {
    return this->_instrList.size();
}