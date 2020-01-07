#include <string>
#include <vector>
#include <set>

#ifndef _REGULAR_EXPR_ENGINE
#define _REGULAR_EXPR_ENGINE 1

class RegExprState {
    int code;
    //std::set<RegExprNode*> 
};

class RegExprEngine {
private:
    std::vector<std::vector<int>> _stateTransTable;
    void CreateTableByFile(const std::string& filePath);
    void CreateTableByExpr(const std::string& expr);

public:
    RegExprEngine(const std::string& input, bool isExprOrFilePath);
    bool IsMatched(const std::string&);  // 当前正则引擎是否匹配该字符串
    int TransferStatus(char);  // 引擎在匹配当前字符后的状态
    virtual ~RegExprEngine() = default;
};

#endif