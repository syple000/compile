#include <vector>
#include <list>
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

    Instruction(Instruction* pre, std::vector<std::string>&& components) {
        InsertInstrAfter(pre, this);
        this->_components = std::move(components);
    }

    void BackFill(int index, const std::string& content) {
        this->_components[index] = content;
    }

    static void LinkInstrs(Instruction* instr1, Instruction* instr2) {
        if (instr1 != nullptr) {
            instr1->_next = instr2;
        }
        if (instr2 != nullptr) {
            instr2->_pre = instr1;
        }
    }

    // 插入指令
    static void InsertInstrAfter(Instruction* pre, Instruction* instr) {
        InsertInstrsAfter(pre, instr, instr);
    }

    // 插入指令列
    static void InsertInstrsAfter(Instruction* pre, Instruction* start, Instruction* end) {
        auto next = pre == nullptr ? nullptr : pre->_next;
        start->_pre = pre;
        end->_next = next;
        if (pre != nullptr) {
            pre->_next = start;
        }
        if (next != nullptr) {
            next->_pre = end;
        }
    }

};

struct BackFillAttr : public CfInfo::Attribute {

    // 回填的vec名，回填的vec: 回填的指令， 回填指令中的位置
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
            instrPos.first->BackFill(instrPos.second, content);
        }
        return true;
    }
};

class InstrFlow {
private:
    Instruction* _head, *_tail;
    std::unordered_map<std::string, Instruction*> _labelInstrMap;
    std::unordered_map<BackFillAttr*, std::vector<std::string>> _nextInstrFillBack;

    int _labelCnt = 0;

    static void destroyInstr(Instruction* instr);

    std::string GetLabel();

public:
    InstrFlow();

    virtual ~InstrFlow();

    void AddNextInstrFillBackInfo(const std::string& listName, BackFillAttr* attr);

    void InsertInstrAfter(Instruction* pre, std::vector<std::string>&& components);

    void InsertInstr(std::vector<std::string>&& components);

    void InsertInstrsAfter(Instruction* pre, Instruction* start, Instruction* end);

    void InsertInstrs(Instruction* start, Instruction* end);

    void Remove(Instruction* instr);

    Instruction* GetFirst();

    Instruction* GetLast();

    Instruction* Next(Instruction* instr);

    Instruction* GetInstrByLabel(const std::string& label);

    void AddLabel(Instruction* instr);

    void Traverse(void(*func)(Instruction*));
};

#endif