#include <stack>

#include "./cf_expr.h"
#include "../io/io.h"
#include "../cmp/cmp.h"

#ifndef CONTEXT_FREE_ENGINE 
#define CONTEXT_FREE_ENGINE 1

struct CfExprState {
    CfExpr* _expr;
    int _index;

    bool operator< (const CfExprState& state) const {
        if (this->_expr == state._expr) {
            return this->_index < state._index;
        } else {
            return this->_expr < state._expr;
        }
    }

};

struct StateTrans {
    
};

class CfEngine {
private:
    // 四个文件： 1. 表达式词法分析文件 2. 表达式分析文件 3. 词法分析文件 4. 分析文件
    CfUtil* _cfUtil;
    std::map<std::set<CfExprState>*, int, SetCmp<CfExprState*>> _stateMap;
    std::vector<std::set<CfExprState>*> _stateVec;
    std::vector<std::vector<StateTrans>> _stateTransTable;
};

#endif