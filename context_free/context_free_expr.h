#include <string>
#include <vector>
#include <set>

#ifndef CONTEXT_FREE
#define CONTEXT_FREE 1

struct ContextFreeExprNode {
    std::string _key;
    std::string _keyRegExpr;
    bool _isTerminator;
    // 0: not nullable; 1: nullable; 2: unknown
    int _nullable;
    int _number;
    std::set<ContextFreeExprNode*> *_first, *_last;

    ContextFreeExprNode(const std::string& key, const std::string& keyRegExpr, bool isTerminator, int nullable, int number) 
        : _key(key), _keyRegExpr(keyRegExpr), _isTerminator(isTerminator), _nullable(nullable), _number(number) {}
};

struct ContextFreeExpr {
    ContextFreeExprNode* _sourceProduction;
    std::vector<ContextFreeExprNode*> _productionBody;
};

struct ContextFreeExprState {
    ContextFreeExpr* expr;
    int index;
};

#endif