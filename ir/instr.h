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
    Instruction* _pre, *_next;
    std::vector<std::string> _components;
    std::string _label;
    // 状态位：标记是否是跳转至，跳出等信息
    int _state;

    Instruction(std::vector<std::string>&& components) : _pre(nullptr), _next(nullptr), _state(-1) {
        this->_components = std::move(components);
    }

    Instruction() : _pre(nullptr), _next(nullptr), _state(-1) {}

    void BackFill(int index, const std::string& content) {
        this->_components[index] = content;
    }
};

class InstrList {
private:
    Instruction* _head, *_tail, *_cur;
    int _size;

    void LinkInstrs(Instruction* instr1, Instruction* instr2);

    static void DestroyInstr(Instruction* instr);

public:
    InstrList();

    virtual ~InstrList();

    void LocFirst();

    void LocLast();

    void SetCurInstr(Instruction* instr);

    Instruction* GetCurInstr();

    Instruction* GoAhead();

    Instruction* GoBack();

    void InsertInstrAfter(Instruction* pre, Instruction* instr);

    void InsertInstr(Instruction* instr);

    void InsertInstrsAfter(Instruction* pre, Instruction* start, Instruction* end, int size);

    void InsertInstrs(Instruction* start, Instruction* end, int size);

    // 参数中的instrList会被清空
    void MergeInstrListAfter(Instruction* pre, InstrList* instrList);

    void MergeInstrList(InstrList* instrList);

    Instruction* RemoveInstr(Instruction* instr);

    Instruction* RemoveInstrs(Instruction* start, Instruction* end);

    void ForwardTraverse(void(*func)(Instruction*));

    void ReverseTraverse(void(*func)(Instruction*));

};

#endif