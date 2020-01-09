#include <string>
#include <vector>
#include <set>
#include <map>
#include "./regexpr_analysis_tree.h"

#ifndef _REGULAR_EXPR_ENGINE
#define _REGULAR_EXPR_ENGINE 1

class RegExprEngine {
private:
    struct SetCmp {
        bool operator() (const std::set<RegExprNode*>* set1, const std::set<RegExprNode*>* set2) const {
            return *set1 < *set2;
        }
    };

    std::vector<std::vector<int>> _stateTransTable;
    std::vector<bool> _terminalStates;
    void CreateTableByFile(const std::string& filePath);
    void CreateTableByExpr(const std::string& expr);
    void AddState(std::map<std::set<RegExprNode*>*, int, RegExprEngine::SetCmp> &statesMap, 
        std::vector<std::set<RegExprNode*>*> &statesVec, std::set<RegExprNode*>* posSet, RegExprNode* terminalState);

public:

    RegExprEngine(const std::string& input, bool isExpr);
    bool IsMatched(const std::string&) const;  // 当前正则引擎是否匹配该字符串
    int TransferStatus(int, unsigned char) const;  // 引擎在匹配当前字符后的状态
    bool InitSuccess();
    std::string StandardizeExpr(const std::string& expr);
    virtual ~RegExprEngine() = default;
};

#endif