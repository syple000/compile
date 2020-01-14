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
struct SiblingExprs;

struct SymbolSubject {
    static bool NullableInfoObserverUpdate(ContextFreeSymbol* const &subjectSymbol, SiblingExprs* &observerExprs) {
        if (observerExprs->_sourceSymbol->_nullable != 2) {
            return true;
        }
        int nullable = 0;
        std::set<ContextFreeSymbol*> dependings;
        std::set<ContextFreeExpr*> dependingExprs;
        for (auto expr : observerExprs->_exprs) {
            std::set<ContextFreeSymbol*> exprDependings;
            int exprNullable = ContextFreeUtil::IsExprNullable(expr, exprDependings);
            if (exprNullable == 1) {
                break;
            } else if (exprNullable == 2) {
                dependings.insert(exprDependings.begin(), exprDependings.end());
                dependingExprs.insert(expr);
                nullable = 2;
            }
        }
        if (nullable != 2) {
            observerExprs->_sourceSymbol->_nullable = nullable;
            observerExprs->_sourceSymbol->_subjects->_nullableInfoSubject.SetUpdated(true);
            observerExprs->_sourceSymbol->_subjects->_nullableInfoSubject.NotifyObservers();
            return true;
        }
        return false;
    }

    static bool FirstInfoObserverUpdate(ContextFreeSymbol* const& subjectSymbol, SiblingExprs* &observerExprs) {
        if (!subjectSymbol->_updated) {
            return false;
        }
        if (ContextFreeUtil::FirstOfSymbol(observerExprs->_sourceSymbol, observerExprs, false)) {
            observerExprs->_sourceSymbol->_subjects->_firstInfoSubject.SetUpdated(true);
            observerExprs->_sourceSymbol->_subjects->_firstInfoSubject.NotifyObservers();
            observerExprs->_sourceSymbol->_updated = false;
        }
        return false;
    }

    Subject<ContextFreeSymbol*, SiblingExprs*> _nullableInfoSubject;
    Subject<ContextFreeSymbol*, SiblingExprs*> _firstInfoSubject;

    SymbolSubject(ContextFreeSymbol* symbol) : _nullableInfoSubject(symbol), _firstInfoSubject(symbol) {}
};

struct ContextFreeSymbol {
    std::string _key;
    std::string _keyRegExpr;
    int _number;
    // 不可空：0， 可空：1， 未知：2
    int _nullable;
    bool _isTerminator;
    // first is updated
    bool _updated = false;
    std::set<ContextFreeSymbol*> _first, _last;
    std::map<ContextFreeExpr*, int> _positionInExpr;
    SymbolSubject *_subjects;

    ContextFreeSymbol(const std::string& key, const std::string& keyRegExpr,  int number, bool isTerminator, int nullable) 
        : _key(key), _keyRegExpr(keyRegExpr), _number(number), _nullable(nullable), _isTerminator(isTerminator) {
        this->_subjects = new SymbolSubject(this);
    }

    virtual ~ContextFreeSymbol() {
        delete this->_subjects;
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

struct SiblingExprs {
    // exprs with same source symbol
    ContextFreeSymbol* _sourceSymbol;
    std::set<ContextFreeExpr*> _exprs;
};

class ContextFreeUtil {
private:
    std::unordered_map<std::string, ContextFreeSymbol*> _symbolMap;
    std::unordered_map<ContextFreeSymbol*, SiblingExprs*> _exprMap;

    void GenNullable();

    void GenFirst();

    void GenLast();

public:

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

    static int IsSymbolNullable(ContextFreeSymbol* symbol, SiblingExprs* exprs) {
        if (symbol->_nullable != 2) {
            return symbol->_nullable;
        }
        std::set<ContextFreeSymbol*> dependings;
        int nullable = 0;
        for (auto expr : exprs->_exprs) {
            std::set<ContextFreeSymbol*> exprDependings;
            int exprNullable = IsExprNullable(expr, exprDependings);
            if (exprNullable == 1) {
                nullable = 1;
                break;
            } else if (exprNullable == 2) {
                dependings.insert(exprDependings.begin(), exprDependings.end());
                nullable = 2;
            }
        }
        if (nullable == 2) {
            for (auto dependingSymbol : dependings) {
                dependingSymbol->_subjects->_nullableInfoSubject.InsertObserver(
                    Observer<ContextFreeSymbol*, SiblingExprs*>(exprs, SymbolSubject::NullableInfoObserverUpdate));
            }
        } else {
            symbol->_nullable = nullable;
            symbol->_subjects->_nullableInfoSubject.SetUpdated(true);
            symbol->_subjects->_nullableInfoSubject.NotifyObservers();
        }
        return nullable;
    }

    static bool FirstOfSymbol(ContextFreeSymbol* symbol, SiblingExprs* exprs, bool isInit) {
        bool updated = false;
        std::set<ContextFreeSymbol*> dependings;
        for (auto expr : exprs->_exprs) {
            updated = updated || FirstOfExpr(expr, dependings, isInit);
        }
        for (auto dependingSymbol : dependings) {
            dependingSymbol->_subjects->_firstInfoSubject.InsertObserver(
                Observer<ContextFreeSymbol*, SiblingExprs*>(exprs, SymbolSubject::FirstInfoObserverUpdate));
        }
        if (updated) {
            symbol->_updated = true;
            symbol->_subjects->_firstInfoSubject.SetUpdated(true);
            symbol->_subjects->_firstInfoSubject.NotifyObservers();
            symbol->_updated = false;
        }
    }

    static bool FirstOfExpr(ContextFreeExpr* expr, std::set<ContextFreeSymbol*>& dependings, bool isInit) {
        ContextFreeSymbol* sourceSymbol = expr->_sourceSymbol;
        int size = sourceSymbol->_first.size();
        for (auto innerSymbol : expr->_production) {
            if (innerSymbol->_updated || isInit) {
                sourceSymbol->_first.insert(innerSymbol->_first.begin(), innerSymbol->_first.end());
            }
            if (!innerSymbol->_isTerminator) {
                dependings.insert(innerSymbol);
            }
            if (innerSymbol->_nullable != 1) {
                break;
            }
        }
        return size != sourceSymbol->_first.size();
    }

};

void ContextFreeUtil::GenNullable() {
    for (auto itr : this->_exprMap) {
        ContextFreeUtil::IsSymbolNullable(itr.first, itr.second);
    }
    // circular dependence exists. the nullable is 0 by constradiction
    for (auto itr : this->_exprMap) {
        if (itr.first->_nullable == 2) {
            itr.first->_nullable = 0;
        }
    }
}

#endif