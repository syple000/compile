
#include <string>
#include <vector>

#include "../string_util/string_util.h"

#ifndef CODE_ANALYSIS_EXPR_ACT_ANALYSIS
#define CODE_ANALYSIS_EXPR_ACT_ANALYSIS

class ExprActAnalysis {
private:
    RegExprEngine _semicolonRegExprEngine;
    RegExprEngine _parenthesesRegExprEngine;
    RegExprEngine _commaRegExprEngine;

    LexicalParser* _lexicalParser;

public:
    // 可继承拓展
    struct Process {
        virtual std::string process(const std::string&) = 0;
    };
    
    ExprActAnalysis();
    virtual ~ExprActAnalysis();
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> GetActsFromStr(const std::string& actStr);
    // 目标函数名： genCode() 当字节点无value时，递归生成
    std::string GenCode(CfTreeNode* root, Process* process);
};

#endif