#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>

#include "../cmp/cmp.h"
#include "./regexpr_analysis_tree.h"

#ifndef _REGULAR_EXPR_ENGINE
#define _REGULAR_EXPR_ENGINE 1

class RegExprEngine {
private:

    std::vector<std::vector<int>> _stateTransTable;
    std::vector<bool> _terminalStates;
    void CreateTableByFile(const std::string& filePath);
    void CreateTableByExpr(const std::string& expr);
    void AddState(std::map<std::set<RegExprNode*>*, int, SetCmp<RegExprNode*>> &statesMap, 
        std::vector<std::set<RegExprNode*>*> &statesVec, std::set<RegExprNode*>* posSet, RegExprNode* terminalState);
    // hook function
    std::string StandardizeExpr(const std::string& expr);

public:

    RegExprEngine(const std::string& input);
    bool IsMatched(const std::string&) const;  // 当前正则引擎是否匹配该字符串
    int TransferState(int, unsigned char) const;  // 引擎在匹配当前字符后的状态
    bool IsTerminalState(int);
    bool InitSuccess();
    virtual ~RegExprEngine() = default;
};

#endif