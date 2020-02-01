
#include "./regexpr_engine.h"

void RegExprEngine::AddState(std::map<std::set<RegExprNode*>*, int, SetCmp<RegExprNode*>> &statesMap, 
    std::vector<std::set<RegExprNode*>*> &statesVec, std::set<RegExprNode*>* posSet, RegExprNode* terminalState) {
    statesMap.insert(std::pair<std::set<RegExprNode*>*, int>(posSet, statesVec.size()));
    statesVec.push_back(posSet);
    this->_terminalStates.push_back(posSet->find(terminalState) != posSet->end());
    this->_stateTransTable.push_back(std::vector<int>(256, -1));
}

void RegExprEngine::CreateTableByExpr(const std::string& expr) {
    std::string finalExpr = RegExprEngine::StandardizeExpr(expr);
    RegExprNode* root = RegExprAnalysisTree().Analyze(finalExpr);
    if (root == nullptr) {
        return;
    }
    RegExprNode* terminalState = root->_rightChildNode;
    std::map<std::set<RegExprNode*>*, int, SetCmp<RegExprNode*>> statesMap;
    std::vector<std::set<RegExprNode*>*> statesVec;
    // init state can't be terminal state
    RegExprEngine::AddState(statesMap, statesVec, root->_first, terminalState);

    int oldStateCount = 0, stateCount = 1;

    while (oldStateCount != stateCount) {
        for (int i = oldStateCount; i < stateCount; i++) {
            std::set<RegExprNode*>* curSet = statesVec[i];
            for (RegExprNode* elem : *curSet) {
                auto it = statesMap.find(elem->_next);
                if (it == statesMap.end()) {
                    RegExprEngine::AddState(statesMap, statesVec, elem->_next, terminalState);
                    this->_stateTransTable[i][(int)elem->_content] = statesVec.size() - 1;
                } else {
                    this->_stateTransTable[i][(int)elem->_content] = it->second;
                }
            }
        }
        oldStateCount = stateCount;
        stateCount = statesVec.size();
    }
    RegExprNode::DestroyTree(root);
}

RegExprEngine::RegExprEngine(const std::string& input) {
    RegExprEngine::CreateTableByExpr(input);
}

bool RegExprEngine::IsMatched(const std::string& str) const {
    int curState = 0;
    for (int i = 0; i < str.size(); i++) {
        curState = RegExprEngine::TransferState(curState, str[i]);
        if (curState == -1) {
            return false;
        }
    }
    return this->_terminalStates[curState];
}

int RegExprEngine::TransferState(int curState, unsigned char ch) const {
    return this->_stateTransTable[curState][(int)ch];
}

bool RegExprEngine::InitSuccess() {
    return this->_stateTransTable.size() != 0;
}

bool RegExprEngine::IsTerminalState(int state) {
    return this->_terminalStates[state];
}

// hook function： 当前cf expr 解析需要功能： []支持; \s解析支持
std::string RegExprEngine::StandardizeExpr(const std::string& expr) {
    // 支持[]操作;支持\s表示空格
    std::stack<std::string> strStack;
    strStack.push("");
    int index = 0;
    while (index < expr.size() && strStack.size() > 0) {
        if (expr[index] == '[') {
            strStack.push("");
        } else if (expr[index] == ']') {
            std::string str = strStack.top();
            strStack.pop();
            strStack.top() += str;
        } else if (expr[index] == '\\') {
            index++;
            // []括号转义需要还原
            if (expr[index] == '[' || expr[index] == ']') {
                strStack.top() += expr[index];
            } else if (expr[index] == 's') {
                // \s需要转义成 ' '
                strStack.top() += ' ';
            } else {
                strStack.top() += '\\';
                strStack.top() += expr[index];
            }
        } else if (expr[index] == '-') {
            // []外的-表示原义, []内-两侧不允许转义
            if (strStack.size() == 1) {
                strStack.top() += expr[index];    
            } else {
                unsigned char endCh = expr[++index];
                unsigned char startCh = strStack.top()[strStack.top().size() - 1];
                strStack.top()[strStack.top().size() - 1] = '(';
                for (; startCh < endCh; startCh++) {
                    strStack.top() += startCh;
                    strStack.top() += '|';
                }
                strStack.top() += startCh;
                strStack.top() += ')';
            }
        } else {
            strStack.top() += expr[index];
        }
        index++;
    }
    if (strStack.size() == 1) {
        return strStack.top();
    } else {
        return "";
    }
}

