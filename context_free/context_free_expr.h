#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

#ifndef CONTEXT_FREE
#define CONTEXT_FREE 1

struct ContextFreeExpr;

struct ContextFreeSymbol {
    std::string _key;
    std::string _keyRegExpr;
    bool _isTerminator;
    // 不可空：0， 可空：1， 未知：2
    int _nullable;
    int _number;

    std::set<ContextFreeSymbol*> *_first, *_last;
    std::map<ContextFreeExpr*, int>* _positionInExpr;

    ContextFreeSymbol(const std::string& key, const std::string& keyRegExpr, bool isTerminator, int nullable, int number) 
        : _key(key), _keyRegExpr(keyRegExpr), _isTerminator(isTerminator), _nullable(nullable), _number(number) {
        this->_first = new std::set<ContextFreeSymbol*>();
        if (this->_isTerminator) {
            this->_last = nullptr;
            this->_positionInExpr = nullptr;
            this->_first->insert(this);
        } else {
            this->_last = new std::set<ContextFreeSymbol*>();
            this->_positionInExpr = new std::map<ContextFreeExpr*, int>();
        }
    }

    ~ContextFreeSymbol() {
        delete this->_first;
        delete this->_last;
        delete this->_positionInExpr;
    }
};

struct ContextFreeExpr {
    ContextFreeSymbol* _sourceSymbol;
    std::vector<ContextFreeSymbol*> _production;
    // 不可空：0， 可空：1， 未知：2
    int _nullable;
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

    int IsNullable(ContextFreeSymbol* symbol, std::set<ContextFreeSymbol*>& dependentSymbols);
    int IsExprNullable(ContextFreeExpr* expr, std::set<ContextFreeSymbol*>& dependentSymbols);
    void GenFirstOfExprNode(const std::string& key);
    void GenLastOfExprNode(const std::string& key);
    void GetFirstOfProductionBody(const std::vector<ContextFreeSymbol*>& symbolVec, int index);
};

#endif