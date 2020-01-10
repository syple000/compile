#include <string>

#ifndef CONTEXT_FREE
#define CONTEXT_FREE 1

struct ContextFreeExprNode {
    std::string _key;
    std::string _keyRegExpr;
    bool _isTerminator;
    // 
    int _nullable;

    ContextFreeExprNode(const std::string& key, const std::string& keyRegExpr, bool isTerminator, bool nullable) 
        : _key(key), _keyRegExpr(keyRegExpr), _isTerminator(isTerminator), _nullable(nullable) {}
};

struct ContextFreeExpr {
    ContextFreeExprNode* _sourceProduction;

};

#endif