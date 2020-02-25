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
        auto itr = exprPos._expr->_actions.find(exprPos._index == exprPos._expr->_production.size() ? -1 : exprPos._index + 1);
        std::string action = itr == exprPos._expr->_actions.end() ? "" : itr->second;
        if (exprPos._index == exprPos._expr->_production.size()) {
            for (auto reducedSymbol : exprPos._expr->_sourceSymbol->_next) {
                if (this->_stateTransInfoTable[state->_number][reducedSymbol->_number]._reducedExpr == nullptr || 
                    this->_stateTransInfoTable[state->_number][reducedSymbol->_number]._reducedExpr != 
                    GetHighPriorityExpr(this->_stateTransInfoTable[state->_number][reducedSymbol->_number]._reducedExpr, exprPos._expr)) {
                    this->_stateTransInfoTable[state->_number][reducedSymbol->_number]._reducedExpr = exprPos._expr;
                    this->_stateTransInfoTable[state->_number][reducedSymbol->_number]._reducedAction = action;
                }
            }
            continue;
        }
        CfSymbol* symbol = exprPos._expr->_production[exprPos._index];
        if (newStates.find(symbol->_number) == newStates.end()) {
            newStates.insert(std::pair<int, CfState*>(symbol->_number, new CfState()));
        }
        auto newState = newStates.find(symbol->_number)->second;
        newState->_exprPosSet.insert(CfExprPos(exprPos._expr, exprPos._index + 1));
        if (GetHighPriorityAction(newState->_action, action) == 1) {
            newState->_action = action;
        }
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
        this->_stateTransInfoTable[state->_number][itr.first]._nextState = nextState->_number;
        this->_stateTransInfoTable[state->_number][itr.first]._action = nextState->_action;
    }
}

CfState* CfEngine::AddState(CfState* state) {
    state->_number = this->_stateVec.size();
    this->_stateSet.insert(state);
    this->_stateVec.push_back(state);
    this->_stateTransInfoTable.push_back(std::vector<StateTransInfo>(this->_cfUtil->GetSymbolCount(), StateTransInfo()));
    return state;
}

bool CfEngine::InitSuccess() {
    return this->_stateTransInfoTable.size() != 0;
}

void CfEngine::ReadCodeLexical(Buffer& codeLexicalBuf) {
    std::map<std::string, std::pair<std::string, int>> keyRegExprMap;
    while (codeLexicalBuf.CurrentCharAvailable()) {
        std::vector<std::string> strVec = codeLexicalBuf.GetStringsOfNextLine();

#ifdef DEBUG_CODE
        if (strVec.size() != 3) {
            if (strVec.size() != 0) {
                std::cout << "code lexical line: " << codeLexicalBuf._curLine << " error!" << std::endl;
            }
            continue;
        }
#endif

        keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(strVec[0], 
            std::pair<std::string, int>(strVec[1], std::stoi(strVec[2]))));
    }
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_END_SYMBOL_", std::pair<std::string, int>("_END_SYMBOL_", 1)));
    this->_lexicalParser = new LexicalParser(keyRegExprMap);
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

#ifdef GEN_AUX_CODE_FILE
    // 注册辅助函数
    this->_auxCode.registFuncs();
#endif

    // reduce first nonterminal symbol
    // key, key reg expr, priority
    if (io.ReadFile(lexicalBuf, lexicalFile) != 0) {
        return;
    }
    ReadCodeLexical(lexicalBuf);

    // 生成状态转换表，可以读取文件加速该过程
    this->_cfUtil->GenInfo();
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
void CfEngine::HandleLexicalError(Buffer& buffer) {
    buffer._curPos = buffer._contentSize;
}

void CfEngine::HandleGrammarError(Buffer& buffer) {
    buffer._curPos = buffer._contentSize;
}

CfTreeNode* CfEngine::GenCfAnalysisTree(const std::string& codeFile) {
    Buffer codeBuf(100);
    IO<std::string> io(String2String, String2String);
    if (io.ReadFile(codeBuf, codeFile) != 0) {
        return nullptr;
    }
    std::stack<int> stateStack;
    std::vector<CfTreeNode*> infoVec;
    stateStack.push(0);
    infoVec.push_back(new CfTreeNode("_START_SYMBOL_", "_START_SYMBOL_", ""));
    CfSymbol* nullSymbol = this->_cfUtil->GetNullSymbol();
    bool matched = true;

    while (codeBuf.CurrentCharAvailable()) {
        int oldPos = codeBuf._curPos;
        std::string key = this->_lexicalParser->GetNextWord(codeBuf);
        if (key == "") {
            if (!codeBuf.CurrentCharAvailable()) {
                break;
            }

            HandleLexicalError(codeBuf);

#ifdef DEBUG_CODE
            std::cout << "lexical error! line: " << codeBuf._curLine << std::endl;
#endif

            continue;
        }
        std::string value = StringUtil::Trim(codeBuf.GetString(oldPos, codeBuf._curPos));
        CfSymbol* symbol = this->_cfUtil->GetCfSymbol(key);
        if (symbol == nullptr) {

#ifdef DEBUG_CODE
            std::cout << "key: " << key << " no correspond value found! line: " << codeBuf._curLine << std::endl;
#endif

            continue;
        }

        matched = ParsingSymbol(stateStack, infoVec, symbol, value);
        if (!matched) {
            HandleGrammarError(codeBuf);

#ifdef DEBUG_CODE
            std::cout << "grammar error! line: " << codeBuf._curLine << std::endl;
#endif

            continue;
        }
    }

    if (matched) {
        CfSymbol* symbol = this->_cfUtil->GetCfSymbol("_END_SYMBOL_");
        matched = ParsingSymbol(stateStack, infoVec, symbol, "_END_SYMBOL_");
    }

    CfTreeNode* root = nullptr;
    int index = 0;
    if (matched) {
        CfTreeNode::DestroyTree(infoVec[0]);
        root = infoVec[1];
        index = 2;
    }

    for (; index < infoVec.size(); index++) {
        CfTreeNode::DestroyTree(infoVec[index]);
    }

    return root;

}

void CfEngine::MoveOn(std::stack<int>& stateStack, std::vector<CfTreeNode*>& infoVec, StateTransInfo& transInfo, const std::string& key, 
    const std::string& value) {
    stateStack.push(transInfo._nextState);
    infoVec.push_back(new CfTreeNode(key, value, transInfo._action));
    ExecuteAction(transInfo._action, nullptr, infoVec);
}

void CfEngine::Reduce(std::stack<int>& stateStack, std::vector<CfTreeNode*>& infoVec, StateTransInfo& transInfo, CfExpr* cfExpr) {
    int symbolCount = cfExpr->_production.size();
    std::vector<CfTreeNode*> cnodes(symbolCount, nullptr);
    for (int i = 0; i < symbolCount; i++) {
        cnodes[symbolCount - i - 1] = infoVec[infoVec.size() - i - 1];
    }
    auto reducedNode = new CfTreeNode(cfExpr->_sourceSymbol->_key, cnodes, cfExpr->_number, "", transInfo._reducedAction);
    ExecuteAction(transInfo._reducedAction, reducedNode, infoVec);

    for (int i = 0; i < symbolCount; i++) {
        infoVec.pop_back();
        stateStack.pop();
    }
    reducedNode->_action = this->_stateTransInfoTable[stateStack.top()][cfExpr->_sourceSymbol->_number]._action;
    int nextState = this->_stateTransInfoTable[stateStack.top()][cfExpr->_sourceSymbol->_number]._nextState;
    stateStack.push(nextState);
    infoVec.push_back(reducedNode);
    ExecuteAction(reducedNode->_action, reducedNode, infoVec);
}

CfExpr* CfEngine::GetHighPriorityExpr(CfExpr* expr1, CfExpr* expr2) {

#ifdef DEBUG_CODE
    for (int i = 0; i < 2; i++) {
        auto expr = i == 0 ? expr1 : expr2;
        std::cout << expr->_sourceSymbol->_key << ":";
        for (auto symbol : expr->_production) {
            std::cout << symbol->_key << " ";
        }
        std::cout << ";   ";
    }
    std::cout << std::endl;
#endif

    return expr1->_reductionPriority < expr2->_reductionPriority ? expr2 : expr1;
}

int CfEngine::GetHighPriorityAction(const std::string& action1, const std::string& action2) {

#ifdef DEBUG_CODE
    if (action2.size() > 0 && action1.size() > 0) {
        std::cout << "action: " << action1 << " and "<< action2 << "conflict!" << std::endl;
    }
#endif

    if (action2.size() == 0 || (action1.size() != 0 && action1 > action2)) {
        return 0;
    } else {
        return 1;
    }
}

CfExpr* CfEngine::GetReduceExpr(const StateTransInfo& transInfo, CfSymbol* symbol) {
    if (transInfo._reducedExpr == nullptr) {
        return nullptr;
    } else {
        if (transInfo._reducedExpr->_reductionFirst.find(symbol) != transInfo._reducedExpr->_reductionFirst.end() || transInfo._nextState == -1) {
            return transInfo._reducedExpr;
        } else {

#ifdef DEBUG_CODE
            if (transInfo._reducedExpr != nullptr) {
                std::cout << "symbol: " << symbol->_key << " reduce: " << transInfo._reducedExpr->_number << " and move on conflict!"; 
            }
#endif

            return nullptr;
        }
    }
}

int CfEngine::StateTrans(std::stack<int>& stateStack, std::vector<CfTreeNode*>& infoVec, CfSymbol* symbol, const std::string& value) {
    StateTransInfo transInfo = this->_stateTransInfoTable[stateStack.top()][symbol->_number];
    CfExpr* expr = GetReduceExpr(transInfo, symbol);
    if (expr == nullptr) {
        if (transInfo._nextState != -1) {
            MoveOn(stateStack, infoVec, transInfo, symbol->_key, value);
            return 0;
        } else {
            return -1;
        }
    } else {
        Reduce(stateStack, infoVec, transInfo, expr);
        return 1;
    }
}

bool CfEngine::ParsingSymbol(std::stack<int>& stateStack, std::vector<CfTreeNode*>& infoVec, CfSymbol* symbol, const std::string& value) {
    while (true) {
        // 非空符号优先
        int stateTransRes = StateTrans(stateStack, infoVec, symbol, value);
        if (stateTransRes == -1) {
            stateTransRes = StateTrans(stateStack, infoVec, this->_cfUtil->GetNullSymbol(), "");
            if (stateTransRes == -1) {
                return false;
            }
        } else if (stateTransRes == 0) {
            return true;
        }
    }
}

CfExpr* CfEngine::GetExpr(int exprNumber) {
    return this->_cfUtil->GetExprByExprNumber(exprNumber);
}

void CfEngine::ExecuteAction(const std::string& action, CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {
    if (action.size() > 0) {
        auto itr = this->_auxCode.funcRegistry.find(action);
        if (itr != this->_auxCode.funcRegistry.end()) {
            itr->second(pnode, knodes);
        } else {

#ifdef DEBUG_CODE
            std::cout << "action: " << action << " not found!" << std::endl;
#endif

        }
    }
}