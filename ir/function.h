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
        : _entities(std::move(entities)) {
        this->_func = (*this->_entities.begin())->_func;
    }
};

template<typename T, typename K>
class FuncGraph {
private:
    // 每一个function都和一个instr对应，具体关系对应与流图一致
    Function<T, K>* _entry, *_exit;
    std::unordered_set<Function<T, K>*> _funcSet;
    void(*_unionOp)(K& dest, const K& src);
    bool(*_exec)(T& obj, const K& inData, K& outData);

    void LabelNumber(Function<T, K>* curFunc, std::vector<Function<T, K>*>& funcVec, int& number);

    std::list<Function<T, K>*> GenTopoSortedFuncList();

    // 循环路径中的函数(约束循环结构最小)
    void GetCyclicFuncSet(Function<T, K>* curFunc, Function<T, K>* cyclicEntry, std::unordered_set<Function<T, K>*>& funcSet);

    // 根据number排序
    std::vector<Function<T, K>*> GetRollbackFuncVec(Function<T, K>* func);

    // 循环入口节点的编号总是循环路径函数中最小
    bool ReplaceCycle(std::list<ExecEntity<T, K>*>& execEntities, Function<T, K>* cyclicEntry,
        const std::unordered_set<Function<T, K>*>& cyclicFuncs, std::list<ExecEntity<T, K>*>& cyclicEntities);

    static K CalcItrExecInData(Function<T, K>* func, void(*unionOp)(K& dest, const K& src));

    static K CalcRegionExecInData(Function<T, K>* func, void(*unionOp)(K& dest, const K& src));

    // 区域执行中的循环执行
    bool LoopExecEntities(const std::list<ExecEntity<T, K>*>& entities, K inData, 
        K(*calcInData)(Function<T, K>*, void(*)(K&, const K&)));

    bool ExecEntities(const std::list<ExecEntity<T, K>*>& entities, K inData, 
        K(*calcInData)(Function<T, K>*, void(*)(K&, const K&)));

    // is region 为 true 需要检查function是否为可归约
    std::list<ExecEntity<T, K>*> GenExecEntities(bool isRegionExec);

    void DestroyExecEntities(const std::list<ExecEntity<T, K>*>& entities);

    std::string GetIndentSpaceStr(int indent);

    void ShowExecEntity(const std::list<ExecEntity<T, K>*>& entities, int indent, void(*showFuncObj)(const T& obj));

public:
    FuncGraph(void(*unionOp)(K&, const K&), bool(*exec)(T&, const K&, K&));

    virtual ~FuncGraph();

    Function<T, K>* GetEntry();

    Function<T, K>* GetExit();

    // 函数需要与entry与exit建立联系
    void AddSuccFunc(Function<T, K>* curFunc, Function<T, K>* succFunc);

    // 迭代执行
    void IterativeExec(const K& initData);

    // 区域执行
    void RegionExec(const K& initData, void(*showFuncObj)(const T&));
};

template<typename T, typename K>
void FuncGraph<T, K>::LabelNumber(Function<T, K>* curFunc, std::vector<Function<T, K>*>& funcVec, int& number) {
    curFunc->_number = 0;
    for (int i = curFunc->_succs.size() - 1; i >= 0; i--) {
        if (curFunc->_succs[i]->_number == -1) {
            LabelNumber(curFunc->_succs[i], funcVec, number);
        }
    }
    curFunc->_number = number--;
    funcVec[curFunc->_number] = curFunc;
}

template<typename T, typename K>
std::list<Function<T, K>*> FuncGraph<T, K>::GenTopoSortedFuncList() {
    int funcNum = this->_funcSet.size() - 1;
    std::vector<Function<T, K>*> funcVec(funcNum + 1, nullptr);
    std::list<Function<T, K>*> funcs;
    LabelNumber(this->_entry, funcVec, funcNum);
    for (auto func : funcVec) {
        funcs.push_back(func);
    }

    return funcs;
}

// 循环路径中的函数(约束循环结构最小)
template<typename T, typename K>
void FuncGraph<T, K>::GetCyclicFuncSet(Function<T, K>* curFunc, Function<T, K>* cyclicEntry, std::unordered_set<Function<T, K>*>& funcSet) {
    funcSet.insert(curFunc);
    if (curFunc != cyclicEntry) {

#ifdef DEBUG_CODE
        if (curFunc->_preds.size() == 0) {
            std::cout << "cyclic is invalid!" << std::endl;
        }
#endif

        for (Function<T, K>* pred : curFunc->_preds) {
            if (funcSet.find(pred) == funcSet.end()) {
                GetCyclicFuncSet(pred, cyclicEntry, funcSet);
            }
        }
    }
}

// 根据number排序
template<typename T, typename K>
std::vector<Function<T, K>*> FuncGraph<T, K>::GetRollbackFuncVec(Function<T, K>* func) {
    std::vector<Function<T, K>*> rollbackFuncVec;
    for (Function<T, K>* succFunc : func->_succs) {
        if (succFunc->_number <= func->_number) {
            rollbackFuncVec.push_back(succFunc);
        }
    }
    std::sort(rollbackFuncVec.begin(), rollbackFuncVec.end());
    return rollbackFuncVec;
}

// 循环入口节点的编号总是循环路径函数中最小
template<typename T, typename K>
bool FuncGraph<T, K>::ReplaceCycle(std::list<ExecEntity<T, K>*>& execEntities, Function<T, K>* cyclicEntry,
    const std::unordered_set<Function<T, K>*>& cyclicFuncs, std::list<ExecEntity<T, K>*>& cyclicEntities) {
    int cnt = execEntities.size();
    auto entityItr = --execEntities.end();
    std::list<ExecEntity<T, K>*> outCyclicEntities;
    bool success = false;
    while (cnt > 0) {
        ExecEntity<T, K>* entity = *entityItr;
        if (cyclicFuncs.find(entity->_func) != cyclicFuncs.end()) {
            cyclicEntities.push_front(entity);
        } else if (entity->_func->_number < cyclicEntry->_number && 
            ReplaceCycle(entity->_entities, cyclicEntry, cyclicFuncs, cyclicEntities)) {
            success = true;
            break;
        } else {
            outCyclicEntities.push_front(entity);
        }
        execEntities.erase(entityItr--);
        if (entity->_func == cyclicEntry) {
            success = true;
            break;
        }
        cnt--;
    }
    if (success && cyclicEntities.size() != 0) {
        execEntities.push_back(new ExecEntity<T, K>(std::move(cyclicEntities)));
        cyclicEntities.clear();
    }
    execEntities.insert(execEntities.end(), outCyclicEntities.begin(), outCyclicEntities.end());
    return success;
}

template<typename T, typename K>
K FuncGraph<T, K>::CalcItrExecInData(Function<T, K>* func, void(*unionOp)(K& dest, const K& src)) {
    K inData;
    for (Function<T, K>* predFunc : func->_preds) {
        unionOp(inData, predFunc->_outData);
    }
    return inData;
}

template<typename T, typename K>
K FuncGraph<T, K>::CalcRegionExecInData(Function<T, K>* func, void(*unionOp)(K& dest, const K& src)) {
    K inData;
    for (Function<T, K>* predFunc : func->_preds) {
        if (predFunc->_number < func->_number) {
            unionOp(inData, predFunc->_outData);
        }
    }
    return inData;
}

// 区域执行中的循环执行
template<typename T, typename K>
bool FuncGraph<T, K>::LoopExecEntities(const std::list<ExecEntity<T, K>*>& entities, K inData, 
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

template<typename T, typename K>
bool FuncGraph<T, K>::ExecEntities(const std::list<ExecEntity<T, K>*>& entities, K inData, 
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
template<typename T, typename K>
std::list<ExecEntity<T, K>*> FuncGraph<T, K>::GenExecEntities(bool isRegionExec) {
    std::list<Function<T, K>*> funcs = GenTopoSortedFuncList();
    std::list<ExecEntity<T, K>*> execEntities;
    if (isRegionExec) {
        for (Function<T, K>* func : funcs) {
            execEntities.push_back(new ExecEntity<T, K>(func));
            std::vector<Function<T, K>*> rollbackFuncs = GetRollbackFuncVec(func);
            for (int i = rollbackFuncs.size() - 1; i >= 0; i--) {
                std::unordered_set<Function<T, K>*> funcSet;
                GetCyclicFuncSet(func, rollbackFuncs[i], funcSet);
                std::list<ExecEntity<T, K>*> cyclicEntities;
                ReplaceCycle(execEntities, rollbackFuncs[i], funcSet, cyclicEntities);
            }
        }
    } else {
        for (Function<T, K>* func : funcs) {
            execEntities.push_back(new ExecEntity<T, K>(func));
        }
    }
    return execEntities;
}

template<typename T, typename K>
void FuncGraph<T, K>::DestroyExecEntities(const std::list<ExecEntity<T, K>*>& entities) {
    for (ExecEntity<T, K>* entity : entities) {
        if (entity->_entities.size() != 0) {
            DestroyExecEntities(entity->_entities);
        }
        delete entity;
    }
}

template<typename T, typename K>
std::string FuncGraph<T, K>::GetIndentSpaceStr(int indent) {
    std::string str;
    for (int i = 0; i < indent; i++) {
        str += ' ';
    }
    return str;
}

template<typename T, typename K>
void FuncGraph<T, K>::ShowExecEntity(const std::list<ExecEntity<T, K>*>& entities, int indent, void(*showFuncObj)(const T& obj)) {
    for (ExecEntity<T, K>* entity : entities) {
        if (entity->_entities.size() != 0) {
            std::cout << GetIndentSpaceStr(indent) << "{" << std::endl;
            ShowExecEntity(entity->_entities, indent + 4, showFuncObj);
            std::cout << GetIndentSpaceStr(indent) << "}" << std::endl;
        } else {
            std::cout << entity->_func->_number << "  " << GetIndentSpaceStr(indent);
            showFuncObj(entity->_func->_object);
            std::cout << std::endl;
        }
    }
}

template<typename T, typename K>
FuncGraph<T, K>::FuncGraph(void(*unionOp)(K&, const K&), bool(*exec)(T&, const K&, K&)) : _unionOp(unionOp), _exec(exec) {
    this->_entry = new Function<T, K>();
    this->_exit = new Function<T, K>();
    this->_funcSet.insert(this->_entry);
    this->_funcSet.insert(this->_exit);
}

template<typename T, typename K>
FuncGraph<T, K>::~FuncGraph() {
    for (Function<T, K>* func : this->_funcSet) {
        delete func;
    }   
}

template<typename T, typename K>
Function<T, K>* FuncGraph<T, K>::GetEntry() {
    return this->_entry;
}

template<typename T, typename K>
Function<T, K>* FuncGraph<T, K>::GetExit() {
    return this->_exit;
}

// 函数需要与entry与exit建立联系
template<typename T, typename K>
void FuncGraph<T, K>::AddSuccFunc(Function<T, K>* curFunc, Function<T, K>* succFunc) {
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
template<typename T, typename K>
void FuncGraph<T, K>::IterativeExec(const K& initData) {
    std::list<ExecEntity<T, K>*> execEntities = GenExecEntities(false);
    while (true) {
        if (!ExecEntities(execEntities, initData, CalcItrExecInData)) {
            break;
        }
    }
    DestroyExecEntities(execEntities);
}

// 区域执行
template<typename T, typename K>
void FuncGraph<T, K>::RegionExec(const K& initData, void(*showFuncObj)(const T&)) {
    std::list<ExecEntity<T, K>*> execEntities = GenExecEntities(true);

#ifdef DEBUG_CODE
        if (showFuncObj != nullptr) {
            std::cout << "region execute entities: " << std::endl; 
            ShowExecEntity(execEntities, 0, showFuncObj);
        }
#endif

    ExecEntities(execEntities, initData, CalcRegionExecInData);
    DestroyExecEntities(execEntities);
}


#endif