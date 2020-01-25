#include "./cf_expr.h"
#include "./lexical_parser.h"

bool SymbolSubject::NullableInfoObserverUpdate(CfSymbol* const &subjectSymbol, SiblingExprs* &observerExprs) {
    if (observerExprs->_sourceSymbol->_nullable != 2) {
        return true;
    }
    int nullable = 0;
    for (auto expr : observerExprs->_exprs) {
        int exprNullable = CfUtil::IsExprNullable(expr, nullptr);
        if (exprNullable == 1) {
            nullable = 1;
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

bool SymbolSubject::FirstInfoObserverUpdate(CfSymbol* const& subjectSymbol, CfSymbol* &observerSymbol) {
    int size = observerSymbol->_first.size();
    observerSymbol->_first.insert(subjectSymbol->_first.begin(), subjectSymbol->_first.end());
    if (size != observerSymbol->_first.size()) {
        observerSymbol->_subjects->_firstInfoSubject.SetUpdated(true);
        observerSymbol->_subjects->_firstInfoSubject.NotifyObservers();
    }
    return false;
}

bool SymbolSubject::NextInfoObserverUpdate(CfSymbol* const& subjectSymbol, CfSymbol* &observerSymbol) {
    int size = observerSymbol->_next.size();
    observerSymbol->_next.insert(subjectSymbol->_next.begin(), subjectSymbol->_next.end());
    if (size != observerSymbol->_next.size()) {
        observerSymbol->_subjects->_nextInfoSubject.SetUpdated(true);
        observerSymbol->_subjects->_nextInfoSubject.NotifyObservers();
    }
    return false;
}

SymbolSubject::SymbolSubject(CfSymbol* symbol) : _nullableInfoSubject(symbol), _firstInfoSubject(symbol), _nextInfoSubject(symbol) {}

CfSymbol::CfSymbol(const std::string& key, const std::string& keyRegExpr,  int number, bool isTerminator, int nullable) 
    : _key(key), _keyRegExpr(keyRegExpr), _number(number), _nullable(nullable), _isTerminator(isTerminator) {
    this->_subjects = new SymbolSubject(this);
}

CfSymbol::~CfSymbol() {
    delete this->_subjects;
}

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
    for (auto symbol : this->_symbolVec) {
        if (symbol->_isTerminator) {
            symbol->_first.insert(symbol);
        }
    }
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

int CfUtil::IsExprNullable(CfExpr* expr, std::set<CfSymbol*>* dependings) {
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

int CfUtil::IsSymbolNullable(CfSymbol* symbol, SiblingExprs* exprs) {
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

bool CfUtil::FirstOfSymbol(CfSymbol* symbol, SiblingExprs* exprs) {
    bool updated = false;
    auto dependings = new std::set<CfSymbol*>();
    for (auto expr : exprs->_exprs) {
        updated = FirstOfExpr(expr, dependings) || updated;
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
    return updated;
}

bool CfUtil::FirstOfExpr(CfExpr* expr, std::set<CfSymbol*>* dependings) {
    CfSymbol* sourceSymbol = expr->_sourceSymbol;
    int size = sourceSymbol->_first.size();
    for (auto innerSymbol : expr->_production) {
        sourceSymbol->_first.insert(innerSymbol->_first.begin(), innerSymbol->_first.end());
        if (!innerSymbol->_isTerminator) {
            dependings->insert(innerSymbol);
        }
        if (innerSymbol->_nullable != 1) {
            break;
        }
    }
    return size != sourceSymbol->_first.size();
}

CfUtil::~CfUtil() {
    for (auto itr : this->_exprMap) {
        auto exprs = itr.second;
        for (auto expr : exprs->_exprs) {
            delete expr;
        }
        delete exprs;
    }

    for (auto symbol : this->_symbolMap) {
        delete symbol.second;
    }
}

// 词法与语法之间有一个空行
CfUtil::CfUtil(Buffer& lexicalBuffer, Buffer& exprBuffer) {
    // key, key_reg_expr, ptiority(数字大，优先级高), is_terminator(1表示是), is_nullable(0,1,2 含义与之前定义一致)
    // #表示注释行
    std::vector<std::string> strVec;
    std::map<std::string, std::pair<std::string, int>> keyRegExprMap;

    while (lexicalBuffer.CurrentCharAvailable()) {
        strVec = lexicalBuffer.GetStringsOfNextLine();
        if (strVec.size() == 0 || strVec[0][0] == '#' || this->_symbolMap.find(strVec[0]) != this->_symbolMap.end()) {
            continue;
        }
        int priority = std::stoi(strVec[2].c_str());
        int isTerminator = std::stoi(strVec[3].c_str());
        int isNullable = std::stoi(strVec[4].c_str());
        CfSymbol* symbol = AddSymbol(strVec[0], strVec[1], this->_symbolVec.size(), isTerminator == 1, isNullable);
        if (symbol->_isTerminator == 1 && symbol->_nullable == 1) {
            // null terminal symbol is unique
            if (this->_nullSymbol == nullptr) {
                this->_nullSymbol = symbol;
            } else {
                return;
            }
        }
        keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(strVec[0], std::pair<std::string, int>(strVec[1], priority)));
    }
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("#", std::pair<std::string, int>("#", 0)));
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_END_SYMBOL_", std::pair<std::string, int>("_END_SYMBOL_", 1)));
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_START_SYMBOL_", std::pair<std::string, int>("_START_SYMBOL_", 1)));
    CfSymbol* endSymbol = AddSymbol("_END_SYMBOL_", "_END_SYMBOL_", this->_symbolVec.size(), true, 0);
    CfSymbol* startSymbol = AddSymbol("_START_SYMBOL_", "_START_SYMBOL_", this->_symbolVec.size(), false, 2);
    LexicalParser lexicalParser = LexicalParser(keyRegExprMap);
    
    // 表达式第一个单词
    std::string initkey = exprBuffer.GetNextStringSplitByBlank();
    auto initSymbolItr = this->_symbolMap.find(initkey);
    if (initSymbolItr == this->_symbolMap.end()) {
        return;
    } else {
        CfExpr* expr = new CfExpr();
        expr->_sourceSymbol = startSymbol;
        expr->_production.push_back(initSymbolItr->second);
        expr->_production.push_back(endSymbol);
        SiblingExprs* exprs = new SiblingExprs();
        exprs->_sourceSymbol = startSymbol;
        exprs->_exprs.insert(expr);
        this->_exprMap.insert(std::pair<CfSymbol*, SiblingExprs*>(expr->_sourceSymbol, exprs));
    }

    this->_initSymbol = startSymbol;

    while (exprBuffer.CurrentCharAvailable()) {
        
        CfExpr* expr = new CfExpr();
        bool isMainBody = true;
        while (exprBuffer.CurrentCharAvailable()) {
            std::string key = lexicalParser.GetNextWord(exprBuffer);
            auto symbolItr = this->_symbolMap.find(key);
            if (symbolItr == this->_symbolMap.end()) {
                if (key == "#") {
                    isMainBody = false;
                } else if (!isMainBody) {
                    expr->_reductionPriority = std::stoi(key);
                } else {
                    delete expr;
                    expr = nullptr;
                    break;
                }
            } else {
                if (isMainBody) {
                    if (expr->_sourceSymbol == nullptr) {
                        expr->_sourceSymbol = symbolItr->second;
                    } else {
                        expr->_production.push_back(symbolItr->second);
                    }
                } else {
                    expr->_reductionFirst.insert(symbolItr->second);
                }
            }
            while (exprBuffer.GetCurrentChar() == ' ' && exprBuffer.CurrentCharAvailable()) {
                exprBuffer.MoveOnByChar();
            }
            if (!exprBuffer.CurrentCharAvailable() || exprBuffer.GetCurrentChar() == '\n') {
                break;
            }
        }
        if (expr == nullptr) {
            // 跳过该行
            exprBuffer.GetNextLine();
            continue;
        }
        auto exprItr = this->_exprMap.find(expr->_sourceSymbol);
        if (exprItr != this->_exprMap.end()) {
            exprItr->second->_exprs.insert(expr);
        } else {
            SiblingExprs* exprs = new SiblingExprs();
            exprs->_sourceSymbol = expr->_sourceSymbol;
            exprs->_exprs.insert(expr);
            this->_exprMap.insert(std::pair<CfSymbol*, SiblingExprs*>(expr->_sourceSymbol, exprs));
        }
    }

    GenNullable();

    GenFirst();

    GenNext();
}

CfSymbol* CfUtil::GetCfSymbol(const std::string& symbolKey) {
    auto itr = this->_symbolMap.find(symbolKey);
    if (itr == this->_symbolMap.end()) {
        return nullptr;
    } else {
        return itr->second;
    }
}

SiblingExprs* CfUtil::GetSiblingExprs(CfSymbol* symbol) {
    auto itr = this->_exprMap.find(symbol);
    if (itr == this->_exprMap.end()) {
        return nullptr;
    } else {
        return itr->second;
    }
}

CfSymbol* CfUtil::GetInitSymbol() {
    return this->_initSymbol;
}

CfSymbol* CfUtil::GetNullSymbol() {
    return this->_nullSymbol;
}

CfSymbol* CfUtil::GetSymbolByIndex(int index) {
    return this->_symbolVec[index];
}

int CfUtil::GetSymbolCount() {
    return this->_symbolVec.size();
}

CfSymbol* CfUtil::AddSymbol(const std::string& key, const std::string& keyRegExpr, int number, bool isTerminator, int nullable) {
    CfSymbol* symbol = new CfSymbol(key, keyRegExpr, number, isTerminator, nullable);
    this->_symbolVec.push_back(symbol);
    this->_symbolMap.insert(std::pair<std::string, CfSymbol*>(key, symbol));
    return symbol;
}