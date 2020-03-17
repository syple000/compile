#include "../ir/function.h"
#include "../ir/instr.h"
#include "../bitset/bitset.h"
#include "../regular_expr/regexpr_engine.h"

struct FuncAdditionInfo {
    std::unordered_map<std::string, int> _varNumMap;
    std::unordered_set<Instruction*> _invalidInstrs;
};

struct FuncObj {
    Instruction* _instr;
    FuncAdditionInfo* _additionInfo;

    FuncObj(Instruction* instr, FuncAdditionInfo* additionInfo) : _instr(instr), _additionInfo(additionInfo) {}

    FuncObj() : _instr(nullptr), _additionInfo(nullptr) {}
};

class VarArrivalAnalysis {
private:
    InstrList *_instrList;
    FuncGraph<FuncObj, BitSet> *_funcGraph;
    FuncAdditionInfo* _addtionInfo;
    std::unordered_map<Instruction*, Function<FuncObj, BitSet>*> _instrFuncMap;

    static void UnionData(BitSet& set1, const BitSet& set2) {
        set1.Union(set2);
    }

    static bool ExecFunc(FuncObj& obj, const BitSet& inData, BitSet& outData) {
        outData = inData;
        bool changed = false;
        if (obj._additionInfo == nullptr && obj._instr == nullptr) {
            return changed;
        }
        auto assignVar = GetAssignVar(obj._instr);
        if (assignVar.size() > 0) {
            int varNum = obj._additionInfo->_varNumMap.find(assignVar)->second;
            if (!outData.Exist(varNum)) {
                outData.Set(varNum);
                changed = true;
            }
        }
        for (auto usedVar : GetUsedVar(obj._instr)) {
            if (!outData.Exist(obj._additionInfo->_varNumMap.find(usedVar)->second)) {
                obj._additionInfo->_invalidInstrs.insert(obj._instr);
            } else {
                obj._additionInfo->_invalidInstrs.erase(obj._instr);
            }
        }
        return changed;
    }

    static int GetOpType(const std::string& op) {
        if (op == "jmp") {
            return 0;
        } else if (op == "jl" || op == "jg" || op == "je") {
            return 1;
        } else if (op == "mov") {
            return 2;
        } else if (op == "add" || op == "sub") {
            return 3;
        } else if (op == "cmp") {
            return 4;
        } else if (op == "ret") {
            return 5;
        } else {
            std::cout << "op type error: " << op << std::endl;
            return -1;
        }
    }

    void AddVarToAdditionInfo(const std::string& var) {
        if (this->_addtionInfo->_varNumMap.find(var) == this->_addtionInfo->_varNumMap.end()) {
            this->_addtionInfo->_varNumMap.insert(std::pair<std::string, int>(var, this->_addtionInfo->_varNumMap.size()));
        }
    }

    static std::string GetAssignVar(Instruction* instr) {
        if (instr->_components[0] == "mov") {
            return instr->_components[1];
        }
        return "";
    }

    static std::vector<std::string> GetUsedVar(Instruction* instr) {
        std::vector<std::string> usedVarVec;
        RegExprEngine numberRegExpr("[0-9][0-9]*");
        int opType = GetOpType(instr->_components[0]);
        if (opType == 2) {
            if (!numberRegExpr.IsMatched(instr->_components[2])) {
                usedVarVec.push_back(instr->_components[2]);
            }
        } else if (opType == 3 || opType == 4) {
            for (int i = 1; i < 3; i++) {
                if (!numberRegExpr.IsMatched(instr->_components[i])) {
                    usedVarVec.push_back(instr->_components[i]);
                }
            }
        }
        return usedVarVec;
    }

    void GenMapInfo() {
        this->_instrList->LocFirst();
        while (true) {
            auto instr = this->_instrList->GetCurInstr();
            if (instr == nullptr) {
                break;
            }
            this->_instrFuncMap.insert(std::pair<Instruction*, Function<FuncObj, BitSet>*>(instr, 
                new Function<FuncObj, BitSet>(FuncObj(instr, this->_addtionInfo), BitSet(8))));
            auto assignVar = GetAssignVar(instr);
            if (assignVar.size() != 0) {
                AddVarToAdditionInfo(assignVar);
            }
            for (auto usedVar : GetUsedVar(instr)) {
                AddVarToAdditionInfo(usedVar);
            }
            this->_instrList->GoAhead();
        }
    }

    void AddSuccFunc(Instruction* instr1, Instruction* instr2) {
        auto func1 = this->_instrFuncMap.find(instr1)->second;
        auto func2 = this->_instrFuncMap.find(instr2)->second;
        this->_funcGraph->AddSuccFunc(func1, func2);
    }

    void LinkWithEntry(Instruction* instr) {
        this->_funcGraph->AddSuccFunc(this->_funcGraph->GetEntry(), this->_instrFuncMap.find(instr)->second);
    }

    void LinkWithExit(Instruction* instr) {
        this->_funcGraph->AddSuccFunc(this->_instrFuncMap.find(instr)->second, this->_funcGraph->GetExit());
    }

    void AddFuncs() {
        this->_instrList->LocFirst();
        Instruction* preInstr = nullptr;
        while (true) {
            Instruction* instr = this->_instrList->GetCurInstr();
            if (instr == nullptr && GetOpType(preInstr->_components[0]) != 0) {
                LinkWithExit(preInstr);
                break;
            }
            if (preInstr == nullptr) {
                LinkWithEntry(instr);
            } else if (GetOpType(preInstr->_components[0]) != 0) {
                AddSuccFunc(preInstr, instr);
            }
            int opType = GetOpType(instr->_components[0]);
            if (opType == 0 || opType == 1) {
                Instruction* nextInstr = this->_instrList->GetInstrByLabel(instr->_components[1]);
                AddSuccFunc(instr, nextInstr);
            } else if (opType == 5) {
                LinkWithExit(instr);
            }
            preInstr = instr;
            this->_instrList->GoAhead();
        }
    }

public:
    VarArrivalAnalysis(const std::string& instrFile) {
        this->_instrList = new InstrList(instrFile);
        this->_addtionInfo = new FuncAdditionInfo();
        this->_funcGraph = new FuncGraph<FuncObj, BitSet>(UnionData, ExecFunc);
        
        GenMapInfo();
        AddFuncs();
    }

    virtual ~VarArrivalAnalysis() {
        delete this->_addtionInfo;
        delete this->_funcGraph;
        delete this->_instrList;
    }

    void ItrExec() {
        this->_funcGraph->IterativeExec(BitSet(8));
        std::cout << "invalid instrs:" << std::endl;
        for (auto instr : this->_addtionInfo->_invalidInstrs) {
            if (instr->_label.size() != 0) {
                std::cout << instr->_label << ": ";
            }
            for (auto component : instr->_components) {
                std::cout << component << " ";
            }
            std::cout << std::endl;
        }
    }

    void RegionExec() {
        this->_funcGraph->RegionExec(BitSet(8));
        std::cout << "invalid instrs:" << std::endl;
        for (auto instr : this->_addtionInfo->_invalidInstrs) {
            if (instr->_label.size() != 0) {
                std::cout << instr->_label << ": ";
            }
            for (auto component : instr->_components) {
                std::cout << component << " ";
            }
            std::cout << std::endl;
        }

    }
};