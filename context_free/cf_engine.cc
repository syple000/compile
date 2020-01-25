#include "./cf_engine.h"

std::set<CfExprPos> CfEngine::CalcCfExprPosSetClosure(const std::set<CfExprPos>& basicState) {
    std::set<CfExprPos> stateSet;
    std::vector<CfExprPos> stateVec;
    stateSet.insert(basicState.begin(), basicState.end());
    for (auto& state : basicState) {
        stateVec.push_back(state);
    }
    int oldSize = 0;
    while (oldSize < stateVec.size()) {
        int size = stateVec.size();
        for (; oldSize < size; oldSize++) {
            if (stateVec[oldSize]._index == stateVec[oldSize]._expr->_production.size()) {
                continue;
            }
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

void CfEngine::GenRelatedCfState(const CfState* state) {
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
        this->_stateTransTable[state->_number][itr.first]._nextState = nextState->_number;
    }
}

CfState* CfEngine::AddState(CfState* state) {
    state->_number = this->_stateVec.size();
    this->_stateSet.insert(state);
    this->_stateVec.push_back(state);
    this->_stateTransTable.push_back(std::vector<StateTrans>(this->_cfUtil->GetSymbolCount(), StateTrans()));
    return state;
}

bool CfEngine::InitSuccess() {
    return this->_stateTransTable.size() != 0;
}

// lexical file is explanation of lexical parsing
CfEngine::CfEngine(const std::string& exprLexicalFile, const std::string& exprFile, const std::string& lexicalFile) {
    IO<std::string> io(String2String, String2String);
    Buffer exprLexicalBuf(100), exprBuf(100), lexicalBuf(100);
    if (io.ReadFile(exprLexicalBuf, exprLexicalFile) != 0 || io.ReadFile(exprBuf, exprFile) != 0) {
        return;
    }
    this->_cfUtil = new CfUtil(exprLexicalBuf, exprBuf);
    if (this->_cfUtil->GetInitSymbol() == nullptr) {
        return;
    }
    // reduce first nonterminal symbol
    // key, key reg expr, priority
    if (io.ReadFile(lexicalBuf, lexicalFile) != 0) {
        return;
    }
    std::map<std::string, std::pair<std::string, int>> keyRegExprMap;
    while (lexicalBuf.CurrentCharAvailable()) {
        std::vector<std::string> strVec = lexicalBuf.GetStringsOfNextLine();
        if (strVec.size() == 0 || strVec[0][0] == '#' || this->_cfUtil->GetCfSymbol(strVec[0]) == nullptr) {
            continue;
        }
        keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(strVec[0], 
            std::pair<std::string, int>(strVec[1], std::stoi(strVec[2]))));
    }
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_END_SYMBOL_", std::pair<std::string, int>("_END_SYMBOL_", 1)));
    this->_lexicalParser = new LexicalParser(keyRegExprMap);

    CfState* initState = new CfState();
    for (auto expr : this->_cfUtil->GetSiblingExprs(this->_cfUtil->GetInitSymbol())->_exprs) {
        initState->_exprPosSet.insert(CfExprPos(expr, 0));
    }
    AddState(initState);
    int oldSize = 0;
    while (oldSize < this->_stateVec.size()) {
        int size = this->_stateVec.size();
        for (; oldSize < size; oldSize++) {
            GenRelatedCfState(this->_stateVec[oldSize]);
        }
    }
}

CfEngine::~CfEngine() {
    delete this->_cfUtil;
    delete this->_lexicalParser;
    for (auto state : this->_stateVec) {
        delete state;
    }
}

// hook function
// if there is a comment return true, else return false;
bool CfEngine::HandleComment(const std::string& key, Buffer& buffer) {
    return false;
}

void CfEngine::HandleLexicalError(Buffer& buffer) {
    buffer._curPos = buffer._contentSize;
}

void CfEngine::HandleGrammarError(Buffer& buffer) {
    buffer._curPos = buffer._contentSize;
}

CfTreeNode* CfEngine::GenCfAnalysisTree(const std::string& codeFile) {
    Buffer codeBuf(100);
    IO<std::string> io(String2String, String2String);
    io.ReadFile(codeBuf, codeFile);
    std::stack<StackInfo> infoStack;
    infoStack.push(StackInfo(0, "_START_SYMBOL_", "_START_SYMBOL_"));
    CfSymbol* nullSymbol = this->_cfUtil->GetNullSymbol();
    while (codeBuf.CurrentCharAvailable()) {
        int oldPos = codeBuf._curPos;
        std::string key = this->_lexicalParser->GetNextWord(codeBuf);
        std::string value = codeBuf.GetString(oldPos, codeBuf._curPos);
        if (HandleComment(key, codeBuf)) {
            continue;
        }
        CfSymbol* symbol = this->_cfUtil->GetCfSymbol(key);
        if (symbol == nullptr) {
            HandleLexicalError(codeBuf);
            continue;
        }

        StateTrans info = this->_stateTransTable[infoStack.top()._state][symbol->_number];
        if (info._nextState == -1 && info._reducedExpr.size() == 0) {
            if (nullSymbol != nullptr) {
                // skip null symbol
                info = this->_stateTransTable[infoStack.top()._state][nullSymbol->_number];
                if (info._nextState == -1 && info._reducedExpr.size() == 0) {
                    HandleGrammarError(codeBuf);
                    continue;
                } else {
                    if (info._nextState != -1) {
                        infoStack.push(StackInfo(info._nextState, nullSymbol->_key, ""));
                    } else {

                    }
                }
            } else {
                HandleGrammarError(codeBuf);
                continue;
            }
        }


        if (info._nextState != -1) {
            if (info._reducedExpr.size() == 0) {
                infoStack.
            }
        }
    }
}

void CfEngine::MoveOn(std::stack<StackInfo>& infoStack, int nextState, const std::string& key, const std::string& value) {
    infoStack.push(StackInfo(nextState, key, value));
}

void CfEngine::Reduce(std::stack<StackInfo>& infoStack, CfExpr* cfExpr) {
    
}

CfExpr* CfEngine::GetMaxReductionPriorityExpr(const std::set<CfExpr*>& exprs) {
    CfExpr* expr = *exprs.begin();
    for (auto exprItr : exprs) {
        if (expr->_reductionPriority < exprItr->_reductionPriority) {
            expr = exprItr;
        }
    }
    return expr;
}