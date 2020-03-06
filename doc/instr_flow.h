#include "../ir/instr.h"

#ifndef INSTR_FLOW
#define INSTR_FLOW 1

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
    InstrList* _instrList;
    std::unordered_map<std::string, Instruction*> _labelInstrMap;
    std::unordered_map<BackFillAttr*, std::vector<std::string>> _nextInstrFillBack;

    int _labelCnt = 0;

    std::string GetLabel() {
        return "label_" + std::to_string(this->_labelCnt++);
    }
    
public:

    Instruction* GetLast() {
        return this->_instrList->GetCurInstr();
    }

    InstrFlow() {
        this->_instrList = new InstrList();
    }

    ~InstrFlow() {
        delete this->_instrList;
    }

    void AddNextInstrFillBackInfo(const std::string& listName, BackFillAttr* attr) {
        if (this->_nextInstrFillBack.find(attr) == this->_nextInstrFillBack.end()) {
            this->_nextInstrFillBack.insert(std::pair<BackFillAttr*, std::vector<std::string>>(attr, std::vector<std::string>()));
        }
        this->_nextInstrFillBack.find(attr)->second.push_back(listName);
    }

    void InsertInstr(std::vector<std::string>&& components) {
        auto instr = new Instruction(std::move(components));
        this->_instrList->InsertInstr(instr);
        this->_instrList->GoAhead();
        if (this->_nextInstrFillBack.size() != 0) {
            instr->_label = GetLabel();
            for (auto listNameAttrPair : this->_nextInstrFillBack) {
                for (auto listName : listNameAttrPair.second) {
                    listNameAttrPair.first->BackFill(listName, instr->_label);
                }
            }
            this->_nextInstrFillBack.clear();
        }
    }

    Instruction* GetInstrByLabel(const std::string& label) {
        auto labelInstrPair = this->_labelInstrMap.find(label);
        if (labelInstrPair == this->_labelInstrMap.end()) {
            return nullptr;
        } else {
            return labelInstrPair->second;
        }
    }

    void AddLabel(Instruction* instr) {
        auto label = GetLabel();
        instr->_label = label;
        this->_labelInstrMap.insert(std::pair<std::string, Instruction*>(label, instr));
    } 

    void ForwardTraverse(void(*func)(Instruction*)) {
        this->_instrList->ForwardTraverse(func);
    }

    void ReverseTraverse(void(*func)(Instruction*)) {
        this->_instrList->ReverseTraverse(func);
    }
};

#endif