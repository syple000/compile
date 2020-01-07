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

void RegExprEngine::CreateTableByExpr(const std::string& expr) {
    RegExprNode* root = RegExprAnalysisTree().Analyze(expr);
    if (root == nullptr) {
        return;
    }
    std::map<std::set<RegExprNode*>, int> states;
    states.insert(std::pair<std::set<RegExprNode*>, int>(root->_first, 0));
    

}