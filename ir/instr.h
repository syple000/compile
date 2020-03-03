#include <vector>
#include <list>
#include "../context_free/cf_analysis_info.h"

#ifndef IR_INSTR
#define IR_INSTR 1

struct Instruction {
    Instruction* _pre, *_next = nullptr;
    std::vector<std::string> _components;
    bool _isLabel;

    Instruction(Instruction* pre, std::vector<std::string>&& components) : _pre(pre) {
        if (pre != nullptr) {
            auto next = pre->_next;
            pre->_next = this;
            if (next != nullptr) {
                next->_pre = this;
            }
            this->_next = next;
        }
        this->_isLabel = false;
        this->_components = std::move(components);
    }

    Instruction(Instruction* pre, const std::string& label) : _pre(pre) {
        if (pre != nullptr) {
            auto next = pre->_next;
            pre->_next = this;
            if (next != nullptr) {
                next->_pre = this;
            }
            this->_next = next;
        }
        this->_isLabel = true;
        this->_components.push_back(label);
    }

    void BackFill(int index, const std::string& content) {
        this->_components[index] = content;
    }

};

struct BackFillAttr : public CfInfo::Attribute {

    // 回填的vec名，回填的vec: 回填的指令， 回填指令中的位置
    // B -> B1 || B2
    std::unordered_map<std::string, std::list<std::pair<Instruction*, int>>> _vecMap;

    BackFillAttr() : CfInfo::Attribute("back_fill") {}

    void AddListElem(const std::string& listName, Instruction* instr, int index) {
        if (this->_vecMap.find(listName) == this->_vecMap.end()) {
            this->_vecMap.insert(std::pair<std::string, std::list<std::pair<Instruction*, int>>>(listName, std::list<std::pair<Instruction*, int>>()));
        }
        this->_vecMap.find(listName)->second.push_back(std::pair<Instruction*, int>(instr, index));
    }

    void AddAllListElems(const std::string& listName, BackFillAttr* srcAttr) {
        auto listItr = srcAttr->_vecMap.find(listName);
        if (listItr == srcAttr->_vecMap.end()) {
            return;
        }
        if (this->_vecMap.find(listName) == this->_vecMap.end()) {
            this->_vecMap.insert(std::pair<std::string, std::list<std::pair<Instruction*, int>>>(listName, std::list<std::pair<Instruction*, int>>()));
        }
        this->_vecMap.find(listName)->second.merge(listItr->second);
    }

    void RemoveList(const std::string& listName) {
        this->_vecMap.erase(listName);
    }

    bool BackFill(const std::string& listName, const std::string& content) {
        auto instrList = this->_vecMap.find(listName);
        if (instrList == this->_vecMap.end()) {
            return false;
        }
        for (auto instrPos : instrList->second) {
            instrPos.first->_components[instrPos.second] = content;
        }
        return true;
    }
};

class InstrFlow {
private:
    Instruction* _head, *_tail;
    int _labelCnt = 0;

    static void destroyInstr(Instruction* instr) {
        delete instr;
    }

public:
    InstrFlow() : _head(nullptr), _tail(nullptr) {}

    virtual ~InstrFlow() {
        Traverse(destroyInstr);
    }

    void InsertInstrAfter(Instruction* pre, std::vector<std::string>&& components) {
        auto newInstr = new Instruction(pre, std::move(components));
        if (newInstr->_next == nullptr) {
            this->_tail = newInstr;
        }
        if (pre == nullptr) {
            this->_head = newInstr;
        }
    }

    void InsertLabelAfter(Instruction* pre, const std::string& label) {
        auto newLabel = new Instruction(pre, label);
        if (newLabel->_next == nullptr) {
            this->_tail = newLabel;
        }
        if (pre == nullptr) {
            this->_head = newLabel;
        }
    }

    void InsertInstr(std::vector<std::string>&& components) {
        InsertInstrAfter(this->_tail, std::move(components));
    }

    void InsertLabel(const std::string& label) {
        InsertLabelAfter(this->_tail, label);
    }

    void Remove(Instruction* instr) {
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

    Instruction* GetHead() {
        return this->_head;
    }

    Instruction* GetTail() {
        return this->_tail;
    }

    Instruction* Next(Instruction* instr) {
        return instr->_next;
    }

    void Traverse(void(*func)(Instruction*)) {
        auto instr = this->_head;
        while (instr != nullptr) {
            auto next = instr->_next;
            func(instr);
            instr = next;
        }
    }

    std::string GetLabel() {
        return "label_" + std::to_string(this->_labelCnt++);
    }
};

#endif