#include <vector>
#include <unordered_set>
#include <algorithm>

#ifndef FUNCTION
#define FUNCTION 1

#ifndef DEBUG_CODE
#define DEBUG_CODE 1
#endif

template<typename T, typename K>
struct Function {
    T _object;
    std::vector<Function<T, K>*> _preds, _succs;
    K _outData;
    int _number = -1;

    // outData初始化顶元素
    Function(const T& object, const K& initOutData) : _object(object), _outData(initOutData) {}

    // 初始化entry与exit使用， T K 提供默认构造函数
    Function() {}

    virtual ~Function() {}

    bool operator<(const Function<T, K>& func) const {
        return this->_number < func._number;
    }
};

template<typename T, typename K>
struct ExecEntity {
    // func: 在entity list不为空时， func为entity list的第一执行函数;在entity list为空时， func为执行体的执行函数
    Function<T, K> *_func;
    std::list<ExecEntity<T, K>*> _entities;
    bool _executed = false;
    K _inData;

    ExecEntity(Function<T, K>* func) : _func(func) {}

    ExecEntity(std::list<ExecEntity<T, K>*>&& entities) 
        : _entities(std::move(entities)), _func((*_entities.begin())->_func) {}
};

template<typename T, typename K>
class FuncGraph {
private:
    // 每一个function都和一个instr对应，具体关系对应与流图一致
    Function<T, K>* _entry, *_exit;
    std::unordered_set<Function<T, K>*> _funcSet;
    void(*_unionOp)(K& dest, const K& src);
    bool(*_exec)(T& obj, const K& inData, K& outData);

    int LabelNumber(Function<T, K>* curFunc, int number) {
        curFunc->_number = 0;
        int numIncr = 1;
        for (int i = 0; i < curFunc->_succs.size(); i++) {
            if (curFunc->_succs[i]->_number == -1) {
                numIncr += LabelNumber(curFunc->_succs[i], number + numIncr);
            }
        }
        curFunc->_number = number;
        return numIncr;
    }

    int GetAcyclicPredFuncCnt(Function<T, K>* func) {
        int cnt = 0;
        for (Function<T, K>* pred : func->_preds) {
            if (pred->_number < func->_number) {
                cnt++;
            }
        }
        return cnt;
    }

    void UpdateDependInfo(std::unordered_map<int, std::unordered_set<Function<T, K>*>>& dependCntInfo, 
        std::unordered_map<Function<T, K>*, int>& funcDependMap, Function<T, K>* func, int newDependCnt, int oldDependCnt) {
        if (oldDependCnt != -1) {
            dependCntInfo.find(oldDependCnt)->second.erase(func);
            funcDependMap.erase(func);
        }
        if (dependCntInfo.find(newDependCnt) == dependCntInfo.end()) {
            dependCntInfo.insert(
                std::pair<int, std::unordered_set<Function<T, K>*>>(newDependCnt, std::unordered_set<Function<T, K>*>()));
        }
        dependCntInfo.find(newDependCnt)->second.insert(func);
        funcDependMap.insert(std::pair<Function<T, K>*, int>(func, newDependCnt));
    }

    std::list<Function<T, K>*> GenTopoSortedFuncList() {
        // 对function进行标号
        assert(LabelNumber(this->_entry, 0) == this->_funcSet.size());

        std::unordered_map<int, std::unordered_set<Function<T, K>*>> dependCntInfo;
        std::unordered_map<Function<T, K>*, int> funcDependMap;
        std::list<Function<T, K>*> sortedFuncList;
        for (Function<T, K>* func : this->_funcSet) {
            UpdateDependInfo(dependCntInfo, funcDependMap, func, GetAcyclicPredFuncCnt(func), -1);
        }

        while (dependCntInfo.find(0)->second.size() != 0) {
            auto funcSet = dependCntInfo.find(0)->second;
            for (Function<T, K>* func : funcSet) {
                dependCntInfo.find(0)->second.erase(func);
                funcDependMap.erase(func);
                for (Function<T, K>* succ : func->_succs) {
                    if (succ->_number > func->_number) {
                        int succDepCnt = funcDependMap.find(succ)->second;
                        UpdateDependInfo(dependCntInfo, funcDependMap, succ, succDepCnt - 1, succDepCnt);
                    }
                }                
                sortedFuncList.push_back(func);
            }
        }

        assert(sortedFuncList.size() == this->_funcSet.size());
        return sortedFuncList;
    }

    // 根据number排序
    std::vector<Function<T, K>*> GetRollbackFuncVec(Function<T, K>* func) {
        std::vector<Function<T, K>*> rollbackFuncVec;
        for (Function<T, K>* succFunc : func->_succs) {
            if (succFunc->_number <= func->_number) {
                rollbackFuncVec.push_back(succFunc);
            }
        }
        std::sort(rollbackFuncVec.begin(), rollbackFuncVec.end());
        return rollbackFuncVec;
    }

    int GetIndexInExecEntityVec(const std::vector<ExecEntity<T, K>*>& execEntityVec, int funcNum) {
        int index = execEntityVec.size() - 1;
        while (index >= 0) {
            if (((ExecEntity<T, K>*)execEntityVec[index])->_func->_number == funcNum) {
                break;
            } else {
                index--;
            }
        }

#ifdef DEBUG_CODE
        if (index < 0) {
            std::cout << "func graph is not reducible" << std::endl;
        }
#endif  

        return index;
    }

    static K CalcItrExecInData(Function<T, K>* func, void(*unionOp)(K& dest, const K& src)) {
        K inData;
        for (Function<T, K>* predFunc : func->_preds) {
            unionOp(inData, predFunc->_outData);
        }
        return inData;
    }

    static K CalcRegionExecInData(Function<T, K>* func, void(*unionOp)(K& dest, const K& src)) {
        K inData;
        for (Function<T, K>* predFunc : func->_preds) {
            if (predFunc->_number < func->_number) {
                unionOp(inData, predFunc->_outData);
            }
        }
        return inData;
    }

    // 区域执行中的循环执行
    bool LoopExecEntities(const std::list<ExecEntity<T, K>*>& entities, K inData, 
        K(*calcInData)(Function<T, K>*, void(*)(K&, const K&))) {
        K loopIndata;
        // 得到循环不变分析结果
        while (true) {
            this->_unionOp(loopIndata, inData);
            if (!ExecEntities(entities, inData, calcInData)) {
                break;
            }
            inData = (*entities.rbegin())->_func->_outData;
        }
        // 循环各个出口的函数约束
        ExecEntities(entities, loopIndata, calcInData);
        // 默认返回true
        return true;
    }

    bool ExecEntities(const std::list<ExecEntity<T, K>*>& entities, K inData, 
        K(*calcInData)(Function<T, K>*, void(*)(K&, const K&))) {
        bool changed = false;
        auto entityItr = entities.begin();
        while (entityItr != entities.end()) {
            ExecEntity<T, K>* entity = *entityItr;
            if (!(entity->_inData == inData) || !entity->_executed) {
                if (entity->_entities.size() == 0) {
                    changed = this->_exec(entity->_func->_object, inData, entity->_func->_outData) || changed;
                } else {
                    changed = LoopExecEntities(entity->_entities, inData, calcInData) || changed;
                }
                entity->_executed = true;
                entity->_inData = inData;
            }
            entityItr++;
            if (entityItr != entities.end()) {
                inData = calcInData((*entityItr)->_func, this->_unionOp);
            }
        }
        return changed;
    }

    // is region 为 true 需要检查function是否为可归约
    std::list<ExecEntity<T, K>*> GenExecEntities(bool isRegionExec) {
        std::list<Function<T, K>*> funcs = GenTopoSortedFuncList();
        std::list<ExecEntity<T, K>*> execEntities;
        // 测试用
        for (auto func : funcs) {
            auto instr = func->_object._instr;
            if (instr == nullptr) {
                continue;
            }
            std::cout << func->_number << "    ";
            if (instr->_label.size() != 0) {
                std::cout << instr->_label << ": ";
            }
            for (auto component : instr->_components) {
                std::cout << component << " ";
            }
            std::cout << std::endl;
        }

        if (isRegionExec) {
        } else {
            for (Function<T, K>* func : funcs) {
                execEntities.push_back(new ExecEntity<T, K>(func));
            }
        }
        return execEntities;
    }

    void DestroyExecEntities(const std::list<ExecEntity<T, K>*>& entities) {
        for (ExecEntity<T, K>* entity : entities) {
            if (entity->_entities.size() != 0) {
                DestroyExecEntities(entity->_entities);
            }
            delete entity;
        }
    }

public:
    FuncGraph(void(*unionOp)(K&, const K&), bool(*exec)(T&, const K&, K&)) : _unionOp(unionOp), _exec(exec) {
        this->_entry = new Function<T, K>();
        this->_exit = new Function<T, K>();
        this->_funcSet.insert(this->_entry);
        this->_funcSet.insert(this->_exit);
    }

    virtual ~FuncGraph() {
        for (Function<T, K>* func : this->_funcSet) {
            delete func;
        }   
    }

    Function<T, K>* GetEntry() {
        return this->_entry;
    }

    Function<T, K>* GetExit() {
        return this->_exit;
    }

    // 函数需要与entry与exit建立联系
    void AddSuccFunc(Function<T, K>* curFunc, Function<T, K>* succFunc) {
        for (auto func : curFunc->_succs) {
            if (func == succFunc) {
                return;
            }
        }
        curFunc->_succs.push_back(succFunc);
        succFunc->_preds.push_back(curFunc);
        // 所有函数在作为后继加入时，加入到func set
        this->_funcSet.insert(succFunc);
    }

    // 迭代执行
    void IterativeExec(const K& initData) {
        std::list<ExecEntity<T, K>*> execEntities = GenExecEntities(false);
        while (true) {
            if (!ExecEntities(execEntities, initData, CalcItrExecInData)) {
                break;
            }
        }
        DestroyExecEntities(execEntities);
    }

    // 区域执行
    void RegionExec(const K& initData) {
        std::list<ExecEntity<T, K>*> execEntities = GenExecEntities(true);
        ExecEntities(execEntities, initData, CalcRegionExecInData);
        DestroyExecEntities(execEntities);
    }
};

#endif