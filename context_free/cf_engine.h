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
    
    std::set<CfExprPos> CalcCfExprPosSetClosure(const std::set<CfExprPos>& basicState) {
        std::set<CfExprPos> stateSet;
        std::vector<CfExprPos> stateVec;
        stateSet.insert(basicState.begin(), basicState.end());
        for (auto& state : basicState) {
            stateVec.push_back(state);
        }
        int oldSize = 0;
        int size = stateVec.size();
        while (oldSize < size) {
            for (; oldSize < size; oldSize++) {
                CfSymbol* symbol = stateVec[oldSize]._expr->_production[stateVec[oldSize]._index];
                if (!symbol->_isTerminator) {
                    SiblingExprs* exprs = _cfUtil->GetSiblingExprs(symbol);
                    for (auto expr : exprs->_exprs) {
                        CfExprPos state(expr, 0);
                        if (stateSet.find(state) == stateSet.end()) {
                            stateVec.push_back(state);
                            stateSet.insert(state);
                        }
                    }
                }
            }
        }
        return stateSet;
    }

    void GenRelatedCfState(const CfState* state) {
        std::set<CfExprPos> exprPosSet = CalcCfExprPosSetClosure(state->_exprPosSet);
        std::map<int, CfState*> newStates;
        for (auto& exprPos : exprPosSet) {
            if (exprPos._index == exprPos._expr->_production.size()) {
                for (auto reducedSymbol : exprPos._expr->_sourceSymbol->_next) {
                    this->_stateTransTable[state->_number][reducedSymbol->_number]._reducedExpr.insert(exprPos._expr);
                }
                continue;
            }
            CfSymbol* symbol = exprPos._expr->_production[exprPos._index];
            if (newStates.find(symbol->_number) == newStates.end()) {
                newStates.insert(std::pair<int, CfState*>(symbol->_number, new CfState()));
            }
            newStates.find(symbol->_number)->second->_exprPosSet.insert(CfExprPos(exprPos._expr, exprPos._index + 1));
        }
        for (auto itr : newStates) {
            auto stateItr = this->_stateSet.find(itr.second);
            CfState* nextState;
            if (stateItr == this->_stateSet.end()) {
                nextState = AddState(itr.second);
            } else {
                delete itr.second;
                nextState = *stateItr;
            }
            this->_stateTransTable[state->_number][itr.first]._nextState.insert(nextState->_number);
        }
    }

    CfState* AddState(CfState* state) {
        state->_number = this->_stateVec.size();
        this->_stateSet.insert(state);
        this->_stateVec.push_back(state);
        this->_stateTransTable.push_back(std::vector<StateTrans>(this->_cfUtil->GetSymbolCount(), StateTrans()));
        return state;
    }

public:

    // lexical file is explanation of lexical parsing
    CfEngine(const std::string& exprLexicalFile, const std::string& exprFile, const std::string& lexicalFile) {
        IO<std::string> io(String2String, String2String);
        Buffer exprLexicalBuf(100), exprBuf(100), lexicalBuf(100);
        io.ReadFile(exprLexicalBuf, exprLexicalFile);
        io.ReadFile(exprBuf, exprFile);
        this->_cfUtil = new CfUtil(exprLexicalBuf, exprBuf);
        if (this->_cfUtil->GetInitSymbol() == nullptr) {
            return;
        }
        // key, key reg expr, priority
        io.ReadFile(lexicalBuf, lexicalFile);
        std::map<std::string, std::pair<std::string, int>> keyRegExprMap;
        while (lexicalBuf.CurrentCharAvailable()) {
            std::vector<std::string> strVec = lexicalBuf.GetStringsOfNextLine();
            if (strVec.size() == 0 || strVec[0][0] == '#' || this->_cfUtil->GetCfSymbol(strVec[0]) == nullptr) {
                continue;
            }
            keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(strVec[0], 
                std::pair<std::string, int>(strVec[1], std::stoi(strVec[2]))));
        }

        this->_lexicalParser = new LexicalParser(keyRegExprMap);
        CfState* initState = new CfState();
        for (auto expr : this->_cfUtil->GetSiblingExprs(this->_cfUtil->GetInitSymbol())->_exprs) {
            initState->_exprPosSet.insert(CfExprPos(expr, 0));
        }
        AddState(initState);
        int oldSize = 0;
        int size = this->_stateVec.size();
        while (oldSize < size) {
            for (; oldSize < size; oldSize++) {
                GenRelatedCfState(this->_stateVec[oldSize]);
            }
        }
    }

    virtual ~CfEngine() {
        delete this->_cfUtil;
        delete this->_lexicalParser;
        for (auto state : this->_stateVec) {
            delete state;
        }
    }
};


#endif