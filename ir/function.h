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

    Function(const T& object) : _object(object) {}

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
    bool(*_exec)(const T& obj, const K& inData, K& outData);

    void GenTopoFuncVec(Function<T, K>* curFunc, std::vector<Function<T, K>*>& funcVec) {
        curFunc->_number = 0;
        for (int i = 0; i < curFunc->_preds.size(); i++) {
            if (curFunc->_preds[i]->_number == -1) {
                GenTopoFuncVec(curFunc->_preds[i], funcVec);
            }
        }
        curFunc->_number = funcVec.size();
        funcVec.push_back(curFunc);
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

    K CalcItrExecInData(Function<T, K>* func) {
        K inData;
        for (Function<T, K>* predFunc : func->_preds) {
            this->_unionOp(inData, predFunc->_outData);
        }
        return inData;
    }

    K CalcRegionExecInData(Function<T, K>* func) {
        K inData;
        for (Function<T, K>* predFunc : func->_preds) {
            if (predFunc->_number < func->_number) {
                this->_unionOp(inData, predFunc->_outData);
            }
        }
        return inData;
    }

    // 区域执行中的循环执行
    bool LoopExecEntityVec(const std::vector<ExecEntity<T, K>*>& entityVec, K inData, K(*calcInData)(Function<T, K>*)) {
        bool changed = false;
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

    bool ExecEntityVec(const std::vector<ExecEntity<T, K>*>& entityVec, K inData, K(*calcInData)(Function<T, K>*)) {
        bool changed = false;
        for (int i = 0; i < entityVec.size(); i++) {
            ExecEntity<T, K>* entity = entityVec[i];
            if (entity->_inData != inData || !entity->_executed) {
                if (entity->_entityVec.size() == 0) {
                    changed = changed || this->_exec(entity->_func->_object, inData, entity->_func->_outData);
                } else {
                    changed = changed || LoopExecEntityVec(entity->_entityVec, inData, calcInData);
                }
                entity->_executed = true;
                entity->_inData = inData;
            }
            if (i + 1 < entityVec.size()) {
                inData = calcInData(entityVec[i + 1]->_func);
            }
        }
        return changed;
    }

    // is region 为 true 需要检查function是否为可归约
    std::vector<ExecEntity<T, K>*> GenExecEntityVec(bool isRegionExec) {
        std::vector<Function<T, K>*> funcVec;
        std::vector<ExecEntity<T, K>*> execEntityVec;
        GenTopoFuncVec(this->_exit, funcVec);
        if (isRegionExec) {
            for (Function<T, K>* func : funcVec) {
                std::vector<Function<T, K>*> rollbackFuncs = GetRollbackFuncVec(func);
                if (rollbackFuncs.size() == 0) {
                    execEntityVec.push_back(new ExecEntity(func));
                } else {
                    for (int i = rollbackFuncs.size() - 1; i >= 0; i--) {
                        int index = GetIndexInExecEntityVec(rollbackFuncs[i]->_number);
                        int size = execEntityVec.size() - index;
                        std::vector<ExecEntity<T, K>*> subExecEntityVec(size, nullptr);
                        for (int j = 0; j < size; j++) {
                            subExecEntityVec[size - 1 - j] = execEntityVec[execEntityVec.size() - 1];
                            execEntityVec.pop_back();
                        }
                        execEntityVec.push_back(new ExecEntity<T, K>(subExecEntityVec));
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
    FuncGraph(void(*unionOp)(K&, const K&), bool(*exec)(const T&, const K&, K&)) : _unionOp(unionOp), _exec(exec) {
        this->_entry = new Function<T, K>();
        this->_exit = new Function<T, K>();
        AddSuccFunc(this->_entry, this->_exit);
        this->_funcSet.insert(this->_entry);
    }

    virtual ~FuncGraph() {
        for (Function<T, K>* func : this->_funcSet) {
            delete func;
        }   
    }

    void AddSuccFunc(Function<T, K>* curFunc, Function<T, K>* succFunc) {
        curFunc->_succs.push_back(succFunc);
        succFunc->_preds.push_back(curFunc);
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