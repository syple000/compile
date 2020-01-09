#include <string>
#include <map>
#include "./regexpr_engine.h"
#include "./regexpr_analysis_tree.h"

void RegExprEngine::AddState(std::map<std::set<RegExprNode*>*, int, RegExprEngine::SetCmp> &statesMap, 
    std::vector<std::set<RegExprNode*>*> &statesVec, std::set<RegExprNode*>* posSet, RegExprNode* terminalState) {
    statesMap.insert(std::pair<std::set<RegExprNode*>*, int>(posSet, statesVec.size()));
    statesVec.push_back(posSet);
    _terminalStates.push_back(posSet->find(terminalState) != posSet->end());
    this->_stateTransTable.push_back(std::vector<int>(256, -1));
}

void RegExprEngine::CreateTableByExpr(const std::string& expr) {
    RegExprNode* root = RegExprAnalysisTree().Analyze(expr);
    if (root == nullptr) {
        return;
    }
    RegExprNode* terminalState = root->_rightChildNode;
    std::map<std::set<RegExprNode*>*, int, RegExprEngine::SetCmp> statesMap;
    std::vector<std::set<RegExprNode*>*> statesVec;
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

RegExprEngine::RegExprEngine(const std::string& input, bool isExpr) {
    if (isExpr) {
        RegExprEngine::CreateTableByExpr(input);
    } else {
        //
    }
}

bool RegExprEngine::IsMatched(const std::string& str) const {
    int curState = 0;
    for (int i = 0; i < str.size(); i++) {
        curState = RegExprEngine::TransferStatus(curState, str[i]);
        if (curState == -1) {
            return false;
        }
    }
    return this->_terminalStates[curState];
}

int RegExprEngine::TransferStatus(int curState, unsigned char ch) const {
    return this->_stateTransTable[curState][(int)ch];
}

bool RegExprEngine::InitSuccess() {
    return this->_stateTransTable.size() != 0;
}

