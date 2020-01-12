#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

#ifndef CONTEXT_FREE
#define CONTEXT_FREE 1

struct ContextFreeExpr;
struct ContextFreeSymbol;

struct SymbolState {
    // 不可空：0， 可空：1， 未知：2
    int _nullable;
    bool _isTerminator;
    std::set<ContextFreeSymbol*> _first, _last;
    std::map<ContextFreeExpr*, int> _positionInExpr;

    SymbolState(int nullable, int isTerminator) : _nullable(nullable), _isTerminator(isTerminator) {}
};

struct ContextFreeSymbol {
    std::string _key;
    std::string _keyRegExpr;
    int _number;
    SymbolState _state;

    ContextFreeSymbol(const std::string& key, const std::string& keyRegExpr,  int number, bool isTerminator, int nullable) 
        : _key(key), _keyRegExpr(keyRegExpr), _number(number), _state(nullable, isTerminator) {} 
};

struct ContextFreeExpr {
    ContextFreeSymbol* _sourceSymbol;
    std::vector<ContextFreeSymbol*> _production;
};

struct ContextFreeExprState {
    ContextFreeExpr* expr;
    int index;
};

class ContextFreeExprUtil {
public:
//private:
    std::unordered_map<std::string, ContextFreeSymbol*> _symbolMap;
    std::unordered_map<std::string, std::set<ContextFreeExpr*>> _exprMap;

    void GenNullable();
    void GenFirstOfExprNode(const std::string& key);
    void GenLastOfExprNode(const std::string& key);
    void GetFirstOfProductionBody(const std::vector<ContextFreeSymbol*>& symbolVec, int index);
};

#endif