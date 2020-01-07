#include <string>
#include <map>
#include "./regexpr_engine.h"
#include "./regexpr_analysis_tree.h"

bool RegExprEngine::IsMatched(const std::string& string) {
    return true;
}

int RegExprEngine::TransferStatus(char ch) {
    return 0;
}

void RegExprEngine::AddState(std::map<std::set<RegExprNode*>*, int, RegExprEngine::SetCmp> &statesMap, 
    std::vector<std::set<RegExprNode*>*> &statesVec, std::set<RegExprNode*>* posSet) {
    statesMap.insert(std::pair<std::set<RegExprNode*>*, int>(posSet, statesVec.size()));
    statesVec.push_back(posSet);
    this->_stateTransTable.push_back(std::vector<int>(256, -1));
}

void RegExprEngine::CreateTableByExpr(const std::string& expr) {
    RegExprNode* root = RegExprAnalysisTree().Analyze(expr);
    if (root == nullptr) {
        return;
    }
    std::map<std::set<RegExprNode*>*, int, RegExprEngine::SetCmp> statesMap;
    std::vector<std::set<RegExprNode*>*> statesVec;
    RegExprEngine::AddState(statesMap, statesVec, root->_first);

    int oldStateCount = 0, stateCount = 1;

    while (oldStateCount != stateCount) {
        for (int i = oldStateCount; i < stateCount; i++) {
            std::set<RegExprNode*>* curSet = statesVec[oldStateCount];
            for (RegExprNode* elem : *curSet) {
                auto it = statesMap.find(elem->_next);
                if (it == statesMap.end()) {
                    // new state
                } else {
                    _stateTransTable[i][(int)elem->_content] = it->second;
                }
            }
        }
    }
}