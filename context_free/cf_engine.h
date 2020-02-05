#include <stack>

#include "./cf_expr.h"
#include "../cmp/cmp.h"
#include "./lexical_parser.h"
#include "../tree_node/tree_node.h"

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

struct StateTransInfo {
    int _nextState = -1;
    std::set<CfExpr*> _reducedExpr;

    StateTransInfo() = default;
};

struct StackInfo {
    CfTreeNode* _cfNode;
    int _state;

    StackInfo(int state, const std::string& key, const std::string& value) : _state(state) {
        this->_cfNode = new CfTreeNode(key, value);
    }

    StackInfo(int state, const std::string& key, const std::vector<CfTreeNode*>& cnodes, int reducedExprNumber, const std::string& reductionAction) 
        : _state(state) {
        this->_cfNode = new CfTreeNode(key, cnodes, reducedExprNumber, reductionAction);
    }
};

/*
    效率提升： 生成转换表由读取文件代替
*/
class CfEngine {
private:
    // 四个文件： 1. 表达式词法分析文件 2. 表达式分析文件 3. 词法分析文件(仅包含对1中非终结字符的正则表达式说明) 4. 分析文件
    CfUtil* _cfUtil = nullptr;
    std::set<CfState*, PointerObjectCmp<CfState>> _stateSet;
    std::vector<CfState*> _stateVec;
    std::vector<std::vector<StateTransInfo>> _stateTransInfoTable;
    LexicalParser* _lexicalParser = nullptr;
    
    std::set<CfExprPos> CalcCfExprPosSetClosure(const std::set<CfExprPos>& basicState);

    void GenRelatedCfState(const CfState* state);

    CfState* AddState(CfState* state);

    void MoveOn(std::stack<StackInfo>& infoStack, int nextState, const std::string& key, const std::string& value);

    void Reduce(std::stack<StackInfo>& infoStack, CfExpr* cfExpr);

    CfExpr* GetMaxReductionPriorityExpr(const std::set<CfExpr*>& exprs);

    CfExpr* GetReduceExpr(const StateTransInfo& info, CfSymbol* symbol);

    int StateTrans(std::stack<StackInfo>& infoStack, CfSymbol* symbol, const std::string& value);

    bool ParsingSymbol(std::stack<StackInfo>& infoStack, CfSymbol* symbol, const std::string& value);

    void ReadCodeLexical(Buffer& codeLexicalBuf);

    // error hook function
    void HandleLexicalError(Buffer& buffer);

    void HandleGrammarError(Buffer& buffer);

    // hook function: 分析中处理context free tree node, 可生成局部代码等
    std::string HandleCfTreeNode(CfTreeNode* root);

public:

    // lexical file is explanation of lexical parsing
    CfEngine(const std::string& exprLexicalFile, const std::string& exprFile, const std::string& lexicalFile);

    virtual ~CfEngine();

    bool InitSuccess();

    CfTreeNode* GenCfAnalysisTree(const std::string& codeFile);

    CfExpr* GetExpr(int exprNumber);

};


#endif