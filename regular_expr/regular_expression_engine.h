#include <string>
#include <vector>

#ifndef _REGULAR_EXPR_ENGINE
#define _REGULAR_EXPR_ENGINE 1

class RegularExpressionEngine {
private:
    std::vector<std::vector<int>> _stateTransTable;

public:
    RegularExpressionEngine(const std::string& expr, bool isExprOrFilePath);
    bool IsMatched(std::string);  // 当前正则引擎是否匹配该字符串
    int TransferStatus(char);  // 引擎在匹配当前字符后的状态
    virtual ~RegularExpressionEngine() = default;
};

#endif