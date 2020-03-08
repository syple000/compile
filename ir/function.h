#include <vector>

template<typename C, typename P, typename T>
class Function {
private:
    // 函数具有后效性均从该对象读出并存储
    T _object;
    // 函数执行需要的系数(本出独立该参数，当函数不不更改object的值，可以将多个函数合并成一个函数，只需要更改系数)
    std::vector<C> _coefficientVec;
    // f1 f2 序列如上，从左至右; f1是f1*f2的前驱， f1*f2是f1的后驱; 多个前驱运算使用joinOp函数
    std::vector<Function<C, P, T>*> _predFuncs;
    std::vector<Function<C, P, T>*> _succFuncs;
    
    // 优化计算效率， 保存前一次计算结果; funcChanged： -1 0 1强度递增  -1表示初始化， 0表示未改变， 1表示有改变
    std::vector<P> _prevInOut;
    int _funcChanged;

    P CalcPredFuncsRes(const P& param) {
        if (this->_predFuncs.size() == 0) {
            return param;
        }
        P res = this->_predFuncs[0]->Execute(param);
        for (int i = 1; i < this->_predFuncs.size(); i++) {
            Join(res, this->_predFuncs[i]->Execute(param));
        }
        return res;
    }

    static bool SetChanged(Function<C, P, T>* func) {
        if (func->_funcChanged == 1) {
            return false;
        }
        func->_funcChanged = 1;
        return true;
    }

    static void UpdateFuncState(Function<C, P, T>* func, bool dir, bool(*updateFunc)(Function<C, P, T>*)) {
        if (!updateFunc(func)) {
            // 状态更新返回false，表示已更新或无需更新，返回
            // 可以避免成环对更新造成的影响
            return;
        }
        auto& funcVec = dir ? func->_succFuncs : func->_predFuncs;
        for (int i = 0; i < funcVec.size(); i++) {
            UpdateFuncState(funcVec[i], dir, updateFunc);
        }
    }

public:

    Function(T object, const std::vector<C>& coefficientVec) {
        // 初始化changed为1
        this->_funcChanged = 1;

        this->_object = object;
        this->_coefficientVec = coefficientVec;
    }

    virtual ~Function() {}

    void AddPredFunc(Function<C, P, T>* func) {
        this->_predFuncs.push_back(func);
        func->_succFuncs.push_back(this);
        UpdateFuncState(this, true, SetChanged);
    }

    void AddSuccFunc(Function<C, P, T>* func) {
        this->_succFuncs.push_back(func);
        func->_predFuncs.push_back(this);
        UpdateFuncState(func, true, SetChanged);
    }

    P Execute(const P& param) {
        if (this->_prevInOut.size() != 0 && this->_prevInOut[0] == param && this->_funcChanged == 0) {
            return this->_prevInOut[1];
        } else {
            this->_funcChanged = 0;
            auto rval = CalCurFuncRes(CalcPredFuncsRes(param));
            if (this->_prevInOut.size() == 0) {
                this->_prevInOut.push_back(param);
                this->_prevInOut.push_back(rval);
            } else {
                this->_prevInOut[0] = param;
                this->_prevInOut[1] = rval;
            }
            return rval;
        }
    }

    // 以下两个工厂函数，可由函数指针取代
    virtual P CalcCurFuncRes(const P& param) = 0;

    // 将src与dest合并, 返回dest
    virtual void Join(P& dest, const P& src) = 0;
};