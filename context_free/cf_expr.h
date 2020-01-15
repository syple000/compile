#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <unordered_map>

#include "../observer_pattern/observer_pattern.h"

#ifndef CONTEXT_FREE
#define CONTEXT_FREE 1

struct CfExpr;
struct CfSymbol;
struct SiblingExprs;

struct SymbolSubject {
    static bool NullableInfoObserverUpdate(CfSymbol* const &subjectSymbol, SiblingExprs* &observerExprs) {
        if (observerExprs->_sourceSymbol->_nullable != 2) {
            return true;
        }
        int nullable = 0;
        for (auto expr : observerExprs->_exprs) {
            int exprNullable = CfUtil::IsExprNullable(expr, nullptr);
            if (exprNullable == 1) {
                break;
            } else if (exprNullable == 2) {
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

    static bool FirstInfoObserverUpdate(CfSymbol* const& subjectSymbol, CfSymbol* &observerSymbol) {
        int size = observerSymbol->_first.size();
        observerSymbol->_first.insert(subjectSymbol->_first.begin(), subjectSymbol->_first.end());
        if (size != observerSymbol->_first.size()) {
            observerSymbol->_subjects->_firstInfoSubject.SetUpdated(true);
            observerSymbol->_subjects->_firstInfoSubject.NotifyObservers();
        }
        return false;
    }

    static bool NextInfoObserverUpdate(CfSymbol* const& subjectSymbol, CfSymbol* &observerSymbol) {
        int size = observerSymbol->_next.size();
        observerSymbol->_next.insert(subjectSymbol->_next.begin(), subjectSymbol->_next.end());
        if (size != observerSymbol->_next.size()) {
            observerSymbol->_subjects->_nextInfoSubject.SetUpdated(true);
            observerSymbol->_subjects->_nextInfoSubject.NotifyObservers();
        }
        return false;
    }

    Subject<CfSymbol*, SiblingExprs*> _nullableInfoSubject;
    Subject<CfSymbol*, CfSymbol*> _firstInfoSubject;
    Subject<CfSymbol*, CfSymbol*> _nextInfoSubject;

    SymbolSubject(CfSymbol* symbol) : _nullableInfoSubject(symbol), _firstInfoSubject(symbol), _nextInfoSubject(symbol) {}
};

struct CfSymbol {
    std::string _key;
    std::string _keyRegExpr;
    int _number;
    // 不可空：0， 可空：1， 未知：2
    int _nullable;
    bool _isTerminator;
    std::set<CfSymbol*> _first, _next;
    std::map<CfExpr*, int> _positionInExpr;
    SymbolSubject *_subjects;

    CfSymbol(const std::string& key, const std::string& keyRegExpr,  int number, bool isTerminator, int nullable) 
        : _key(key), _keyRegExpr(keyRegExpr), _number(number), _nullable(nullable), _isTerminator(isTerminator) {
        this->_subjects = new SymbolSubject(this);
    }

    virtual ~CfSymbol() {
        delete this->_subjects;
    }

};

struct CfExpr {
    CfSymbol* _sourceSymbol;
    std::vector<CfSymbol*> _production;
    int _nullable = 2;
};

struct CfExprState {
    CfExpr* expr;
    int index;
};

struct SiblingExprs {
    // exprs with same source symbol
    CfSymbol* _sourceSymbol;
    std::set<CfExpr*> _exprs;
};

class CfUtil {
private:
    std::unordered_map<std::string, CfSymbol*> _symbolMap;
    std::unordered_map<CfSymbol*, SiblingExprs*> _exprMap;

    void GenNullable();

    void GenFirst();

    void GenNext();

    bool GetFirstOfSymbolVec(std::vector<CfSymbol*>& symbols, int index, std::set<CfSymbol*>& firstSet);

    void GenNextOfSymbol(CfSymbol* symbol);

public:

    static int IsExprNullable(CfExpr* expr, std::set<CfSymbol*>* dependings) {
        if (expr->_nullable != 2) {
            return expr->_nullable;
        }
        int nullable = 1;
        for (auto innerSymbol : expr->_production) {
            if (innerSymbol->_nullable == 0) {
                nullable = 0;
                break;
            } else if (innerSymbol->_nullable == 2){
                if (dependings != nullptr) dependings->insert(innerSymbol);
                nullable = 2;
            }
        }
        return nullable;
    }

    static int IsSymbolNullable(CfSymbol* symbol, SiblingExprs* exprs) {
        if (symbol->_nullable != 2) {
            return symbol->_nullable;
        }
        std::set<CfSymbol*> dependings;
        auto exprDependings = new std::set<CfSymbol*>();
        int nullable = 0;
        for (auto expr : exprs->_exprs) {
            int exprNullable = IsExprNullable(expr, exprDependings);
            if (exprNullable == 1) {
                nullable = 1;
                break;
            } else if (exprNullable == 2) {
                dependings.insert(exprDependings->begin(), exprDependings->end());
                nullable = 2;
            }
            exprDependings->clear();
        }
        delete exprDependings;
        if (nullable == 2) {
            for (auto dependingSymbol : dependings) {
                dependingSymbol->_subjects->_nullableInfoSubject.InsertObserver(
                    Observer<CfSymbol*, SiblingExprs*>(exprs, SymbolSubject::NullableInfoObserverUpdate));
            }
        } else {
            symbol->_nullable = nullable;
            symbol->_subjects->_nullableInfoSubject.SetUpdated(true);
            symbol->_subjects->_nullableInfoSubject.NotifyObservers();
        }
        return nullable;
    }

    static bool FirstOfSymbol(CfSymbol* symbol, SiblingExprs* exprs) {
        bool updated = false;
        auto dependings = new std::set<CfSymbol*>();
        for (auto expr : exprs->_exprs) {
            updated = updated || FirstOfExpr(expr, dependings);
        }
        for (auto dependingSymbol : *dependings) {
            dependingSymbol->_subjects->_firstInfoSubject.InsertObserver(
                Observer<CfSymbol*, CfSymbol*>(symbol, SymbolSubject::FirstInfoObserverUpdate));
        }
        delete dependings;
        if (updated) {
            symbol->_subjects->_firstInfoSubject.SetUpdated(true);
            symbol->_subjects->_firstInfoSubject.NotifyObservers();
        }
    }

    static bool FirstOfExpr(CfExpr* expr, std::set<CfSymbol*>* dependings) {
        CfSymbol* sourceSymbol = expr->_sourceSymbol;
        int size = sourceSymbol->_first.size();
        for (auto innerSymbol : expr->_production) {
            sourceSymbol->_first.insert(innerSymbol->_first.begin(), innerSymbol->_first.end());
            if (!innerSymbol->_isTerminator) {
                if (dependings != nullptr) dependings->insert(innerSymbol);
            }
            if (innerSymbol->_nullable != 1) {
                break;
            }
        }
        return size != sourceSymbol->_first.size();
    }

};

void CfUtil::GenNullable() {
    for (auto itr : this->_exprMap) {
        CfUtil::IsSymbolNullable(itr.first, itr.second);
    }
    // circular dependence exists. the nullable is 0 by constradiction
    for (auto itr : this->_exprMap) {
        if (itr.first->_nullable == 2) {
            itr.first->_nullable = 0;
        }
    }
}

void CfUtil::GenFirst() {
    for (auto itr : this->_exprMap) {
        CfUtil::FirstOfSymbol(itr.first, itr.second);
    }
}

bool CfUtil::GetFirstOfSymbolVec(std::vector<CfSymbol*>& symbols, int index, std::set<CfSymbol*>& firstSet) {
    int i;
    for (i = index; i < symbols.size(); i++) {
        firstSet.insert(symbols[i]->_first.begin(), symbols[i]->_first.end());
        if (symbols[i]->_nullable == 0) {
            break;
        }
    }
    return i == symbols.size();
}

void CfUtil::GenNextOfSymbol(CfSymbol* symbol) {
    int size = symbol->_next.size();
    for (auto itr : symbol->_positionInExpr) {
        // symbol's next set is the first set of the expr
        bool nullable = CfUtil::GetFirstOfSymbolVec(itr.first->_production, itr.second + 1, symbol->_next);
        if (nullable) {
            symbol->_next.insert(itr.first->_sourceSymbol->_next.begin(), itr.first->_sourceSymbol->_next.end());
            itr.first->_sourceSymbol->_subjects->_nextInfoSubject.InsertObserver(
                Observer<CfSymbol*, CfSymbol*>(symbol, SymbolSubject::NextInfoObserverUpdate));
        }
    }
    if (symbol->_next.size() != size) {
        symbol->_subjects->_nextInfoSubject.SetUpdated(true);
        symbol->_subjects->_nextInfoSubject.NotifyObservers();
    }
}

void CfUtil::GenNext() {
    for (auto itr : this->_exprMap) {
        for (auto expr : itr.second->_exprs) {
            for (int i = 0; i < expr->_production.size(); i++) {
                if (!expr->_production[i]->_isTerminator) {
                    expr->_production[i]->_positionInExpr.insert(std::pair<CfExpr*, int>(expr, i));
                }
            }
        }
    }

    for (auto itr : this->_exprMap) {
        CfUtil::GenNextOfSymbol(itr.first);
    }
}

#endif