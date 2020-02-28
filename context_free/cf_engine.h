#include <stack>

#include "./cf_expr.h"
#include "../cmp/cmp.h"
#include "./lexical_parser.h"
#include "./cf_analysis_info.h"
#include "./aux_code/aux_code.h"

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
    std::string _action;

    bool operator< (const CfState& state) const {
        return this->_exprPosSet < state._exprPosSet;
    }
};

struct StateTransInfo {
    int _nextState = -1;
    CfExpr* _reducedExpr = nullptr;
    std::string _reducedAction;
    std::string _action;

    StateTransInfo() = default;
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
    AuxCode _auxCode;
    CfInfo _nullCfInfo;
    
    std::set<CfExprPos> CalcCfExprPosSetClosure(const std::set<CfExprPos>& basicState);

    void GenRelatedCfState(const CfState* state);

    CfState* AddState(CfState* state);

    void MoveOn(std::stack<int>& stateStack, std::vector<CfInfo>& infoVec, StateTransInfo& transInfo, const std::string& key, 
        const std::string& value);

    void Reduce(std::stack<int>& stateStack, std::vector<CfInfo>& infoVec, StateTransInfo& transInfo, CfExpr* cfExpr);

    CfExpr* GetHighPriorityExpr(CfExpr* expr1, CfExpr* expr2);

    int GetHighPriorityAction(const std::string& action1, const std::string& action2);

    CfExpr* GetReduceExpr(const StateTransInfo& info, CfSymbol* symbol);

    int StateTrans(std::stack<int>& stateStack, std::vector<CfInfo>& infoVec, CfSymbol* symbol, const std::string& value);

    bool ParsingSymbol(std::stack<int>& stateStack, std::vector<CfInfo>& infoVec, CfSymbol* symbol, const std::string& value);

    void ReadCodeLexical(Buffer& codeLexicalBuf);

    void ExecuteAction(const std::string& action, CfInfo& pinfo, std::vector<CfInfo>& kinfos);

    // error hook function
    void HandleLexicalError(Buffer& buffer);

    void HandleGrammarError(Buffer& buffer);

public:

    // lexical file is explanation of lexical parsing
    CfEngine(const std::string& exprLexicalFile, const std::string& exprFile, const std::string& lexicalFile);

    virtual ~CfEngine();

    bool InitSuccess();

    void GenCfAnalysisInfo(const std::string& codeFile);
};


#endif