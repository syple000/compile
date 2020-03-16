#include <vector>
#include <list>
#include <unordered_map>

#include "../context_free/cf_analysis_info.h"

#ifndef IR_INSTR
#define IR_INSTR 1

#ifndef DEBUG_CODE
#define DEBUG_CODE 1
#endif

struct Instruction {
    std::vector<std::string> _components;
    std::string _label;
    // 状态位：标记是否是跳转至，跳出等信息
    int _state;

    Instruction(std::vector<std::string>&& components) : _state(-1) {
        this->_components = std::move(components);
    }

    Instruction() : _state(-1) {}

    void BackFill(int index, const std::string& content) {
        this->_components[index] = content;
    }
};

class InstrList {
private:
    std::list<Instruction*> _instrList;
    std::list<Instruction*>::iterator _curItr;

    static void DestroyInstr(Instruction* instr);

public:
    InstrList();

    virtual ~InstrList();

    void LocFirst();

    void LocLast();

    void SetCurInstr(const std::string& label);

    Instruction* GetCurInstr();

    Instruction* GoAhead();

    Instruction* GoBack();

    void InsertInstr(Instruction* instr);

    Instruction* RemoveInstr();

    // 清空被合并列表
    void MergeInstrList(InstrList& instrList);

    void ForwardTraverse(void(*func)(Instruction*));

    void ReverseTraverse(void(*func)(Instruction*));

    int GetSize();

};

#endif