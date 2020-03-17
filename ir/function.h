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
    std::vector<ExecEntity<T, K>*> _entityVec;
    bool _executed = false;
    K _inData;

    ExecEntity(Function<T, K>* func) : _func(func) {}

    ExecEntity(std::vector<ExecEntity<T, K>*>&& entityVec) 
        : _entityVec(std::move(entityVec)), _func(_entityVec[0]->_func) {}
};

template<typename T, typename K>
class FuncGraph {
private:
    // 每一个function都和一个instr对应，具体关系对应与流图一致
    Function<T, K>* _entry, *_exit;
    std::unordered_set<Function<T, K>*> _funcSet;
    void(*_unionOp)(K& dest, const K& src);
    bool(*_exec)(T& obj, const K& inData, K& outData);

    // 区域执行有bug
    int GenTopoFuncVec(Function<T, K>* curFunc, std::vector<Function<T, K>*>& funcVec, int number) {
        curFunc->_number = 0;
        int numIncr = 1;
        for (int i = 0; i < curFunc->_succs.size(); i++) {
            if (curFunc->_succs[i]->_number == -1) {
                numIncr += GenTopoFuncVec(curFunc->_succs[i], funcVec, number + numIncr);
            }
        }
        curFunc->_number = number;
        funcVec[curFunc->_number] = curFunc;
        return numIncr;
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
    bool LoopExecEntityVec(const std::vector<ExecEntity<T, K>*>& entityVec, K inData, 
        K(*calcInData)(Function<T, K>*, void(*)(K&, const K&))) {
        K loopIndata;
        // 得到循环不变分析结果
        while (true) {
            this->_unionOp(loopIndata, inData);
            if (!ExecEntityVec(entityVec, inData, calcInData)) {
                break;
            }
            inData = entityVec[entityVec.size() - 1]->_func->_outData;
        }
        // 循环各个出口的函数约束
        ExecEntityVec(entityVec, loopIndata, calcInData);
        // 默认返回true
        return true;
    }

    bool ExecEntityVec(const std::vector<ExecEntity<T, K>*>& entityVec, K inData, 
        K(*calcInData)(Function<T, K>*, void(*)(K&, const K&))) {
        bool changed = false;
        for (int i = 0; i < entityVec.size(); i++) {
            ExecEntity<T, K>* entity = entityVec[i];
            if (!(entity->_inData == inData) || !entity->_executed) {
                if (entity->_entityVec.size() == 0) {
                    changed = this->_exec(entity->_func->_object, inData, entity->_func->_outData) || changed;
                } else {
                    changed = LoopExecEntityVec(entity->_entityVec, inData, calcInData) || changed;
                }
                entity->_executed = true;
                entity->_inData = inData;
            }
            if (i + 1 < entityVec.size()) {
                inData = calcInData(entityVec[i + 1]->_func, this->_unionOp);
            }
        }
        return changed;
    }

    // is region 为 true 需要检查function是否为可归约
    std::vector<ExecEntity<T, K>*> GenExecEntityVec(bool isRegionExec) {
        std::vector<Function<T, K>*> funcVec(this->_funcSet.size(), nullptr);
        std::vector<ExecEntity<T, K>*> execEntityVec;
        GenTopoFuncVec(this->_entry, funcVec, 0);
        if (isRegionExec) {
            for (Function<T, K>* func : funcVec) {
                std::vector<Function<T, K>*> rollbackFuncs = GetRollbackFuncVec(func);
                if (rollbackFuncs.size() == 0) {
                    execEntityVec.push_back(new ExecEntity<T, K>(func));
                } else {
                    for (int i = rollbackFuncs.size() - 1; i >= 0; i--) {
                        int index = GetIndexInExecEntityVec(execEntityVec, rollbackFuncs[i]->_number);
                        int size = execEntityVec.size() - index;
                        std::vector<ExecEntity<T, K>*> subExecEntityVec(size, nullptr);
                        for (int j = 0; j < size; j++) {
                            subExecEntityVec[size - 1 - j] = execEntityVec[execEntityVec.size() - 1];
                            execEntityVec.pop_back();
                        }
                        execEntityVec.push_back(new ExecEntity<T, K>(std::move(subExecEntityVec)));
                    }
                }
            }
        } else {
            for (Function<T, K>* func : funcVec) {
                execEntityVec.push_back(new ExecEntity<T, K>(func));
            }
        }
        return execEntityVec;
    }

    void DestroyExecEntityVec(const std::vector<ExecEntity<T, K>*>& vec) {
        for (ExecEntity<T, K>* entity : vec) {
            if (entity->_entityVec.size() != 0) {
                DestroyExecEntityVec(entity->_entityVec);
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
        std::vector<ExecEntity<T, K>*> execEntityVec = GenExecEntityVec(false);
        while (true) {
            if (!ExecEntityVec(execEntityVec, initData, CalcItrExecInData)) {
                break;
            }
        }
        DestroyExecEntityVec(execEntityVec);
    }

    // 区域执行
    void RegionExec(const K& initData) {
        std::vector<ExecEntity<T, K>*> execEntityVec = GenExecEntityVec(true);
        ExecEntityVec(execEntityVec, initData, CalcRegionExecInData);
        DestroyExecEntityVec(execEntityVec);
    }
};

#endif