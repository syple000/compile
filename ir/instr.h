#include <string>
#include <vector>
#include <cstdarg>

#ifndef IR_INSTR
#define IR_INSTR 1

struct Instruction {
    Instruction* _pre, *_next;
    std::vector<std::string> _components;
    
    // 表示是否是进入(0)或跳转(1)或普通(2)指令
    int _state = -1;

    Instruction(Instruction* pre, std::vector<std::string>&& components) : _pre(pre) {
        if (pre != nullptr) {
            auto next = pre->_next;
            pre->_next = this;
            if (next != nullptr) {
                next->_pre = this;
            }
            this->_next = next;
        }
        this->_components = std::move(components);
    }

    Instruction(Instruction* pre) : _pre(pre) {
        if (pre != nullptr) {
            auto next = pre->_next;
            pre->_next = this;
            if (next != nullptr) {
                next->_pre = this;
            }
            this->_next = next;
        }
    }

};

class InstrFlow {
private:
    Instruction* _head = nullptr, *_tail = nullptr;

public:
    InstrFlow() {}

    virtual ~InstrFlow() {}

    void InsertAfter(Instruction* pre, std::vector<std::string>&& components) {
        auto newInstr = new Instruction(pre, std::move(components));
        if (newInstr->_next == nullptr) {
            this->_tail = newInstr;
        }
        if (pre == nullptr) {
            this->_head = newInstr;
        }
    }

    void Insert(std::vector<std::string>&& components) {
        InsertAfter(this->_tail, std::move(components));
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
        while(instr != nullptr) {
            func(instr);
            instr = instr->_next;
        }
    }
};

#endif