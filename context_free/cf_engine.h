#include <stack>

#include "./cf_expr.h"
#include "../io/io.h"
#include "../io/format_conversion.h"
#include "../cmp/cmp.h"
#include "./lexical_parser.h"

#ifndef CONTEXT_FREE_ENGINE 
#define CONTEXT_FREE_ENGINE 1

struct CfExprPos {
    CfExpr* _expr;
    int _index;

    CfExprPos(CfExpr* expr, int index) : _expr(expr), _index(index) {}

    bool operator< (const CfExprPos& state) const {
        if (this->_expr == state._expr) {
            return this->_index < state._index;
        } else {
            return this->_expr < state._expr;
        }
    }
};

struct CfState {
    std::set<CfExprPos> _exprPosSet;
    int _number;

    bool operator< (const CfState& state) const {
        return this->_exprPosSet < state._exprPosSet;
    }
};

struct StateTrans {
    std::set<int> _nextState;
    std::set<CfExpr*> _reducedExpr;

    StateTrans() = default;
};

class CfEngine {
private:
    // 四个文件： 1. 表达式词法分析文件 2. 表达式分析文件 3. 词法分析文件 4. 分析文件
    CfUtil* _cfUtil = nullptr;
    std::set<CfState*, PointerObjectCmp<CfState>> _stateSet;
    std::vector<CfState*> _stateVec;
    std::vector<std::vector<StateTrans>> _stateTransTable;
    LexicalParser* _lexicalParser = nullptr;
    
    std::set<CfExprPos> CalcCfExprPosSetClosure(const std::set<CfExprPos>& basicState);

    void GenRelatedCfState(const CfState* state);

    CfState* AddState(CfState* state);

public:

    // lexical file is explanation of lexical parsing
    CfEngine(const std::string& exprLexicalFile, const std::string& exprFile, const std::string& lexicalFile);

    virtual ~CfEngine();

    bool InitSuccess();

};


#endif