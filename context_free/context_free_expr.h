#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <unordered_map>

#include "../observer_pattern/observer_pattern.h"

#ifndef CONTEXT_FREE
#define CONTEXT_FREE 1

struct ContextFreeExpr;
struct ContextFreeSymbol;
struct ContextFreeExprState;
class ContextFreeExprUtil;

struct SymbolCalcCache {
    static bool NullableInfoObserverUpdate(const ContextFreeSymbol* subject, ContextFreeSymbol* observer) {
        
    }

    std::set<ContextFreeSymbol*> _first, _last;
    std::map<ContextFreeExpr*, int> _positionInExpr;
    Subject<ContextFreeSymbol*, ContextFreeSymbol*> _nullableInfoSubject;

    SymbolCalcCache(ContextFreeSymbol* symbol) : _nullableInfoSubject(symbol) {}
};

struct ContextFreeSymbol {
    std::string _key;
    std::string _keyRegExpr;
    int _number;
    // 不可空：0， 可空：1， 未知：2
    int _nullable;
    bool _isTerminator;
    SymbolCalcCache *calcCache = nullptr;

    ContextFreeSymbol(const std::string& key, const std::string& keyRegExpr,  int number, bool isTerminator, int nullable) 
        : _key(key), _keyRegExpr(keyRegExpr), _number(number), _nullable(nullable), _isTerminator(isTerminator) {
        calcCache = new SymbolCalcCache(this);
    } 

    static int IsExprNullable(ContextFreeExpr* expr, std::set<ContextFreeSymbol*>& dependings) {
        if (expr->_nullable != 2) {
            return expr->_nullable;
        }
        int nullable = 1;
        for (auto innerSymbol : expr->_production) {
            if (innerSymbol->_nullable == 0) {
                nullable = 0;
                break;
            } else if (innerSymbol->_nullable == 2){
                dependings.insert(innerSymbol);
                nullable = 2;
            }
        }
        return nullable;
    }

    static int IsSymbolNullable(ContextFreeSymbol* symbol, const std::set<ContextFreeExpr*>& exprs) {
        if (symbol->_nullable != 2) {
            
        }
        std::set<ContextFreeSymbol*> dependings;

    }
};

struct ContextFreeExpr {
    ContextFreeSymbol* _sourceSymbol;
    std::vector<ContextFreeSymbol*> _production;
    int _nullable = 2;
};

struct ContextFreeExprState {
    ContextFreeExpr* expr;
    int index;
};

class ContextFreeExprUtil {
private:
    std::unordered_map<std::string, ContextFreeSymbol*> _symbolMap;
    std::unordered_map<std::string, std::set<ContextFreeExpr*>> _exprMap;

    void GenNullable();

    void GenFirst();

    void GenLast();
};

#endif