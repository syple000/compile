#include "./cf_expr.h"
#include "../string_util/string_util.h"

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

#ifdef DEBUG_CODE
    for (auto symbol : this->_symbolVec) {
        if (symbol->_nullable == 2) {
            std::cout << "symbol: " << symbol->_key << " nullable info error!" << std::endl;
        }  
    }
#endif

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
    if (nullable != 2) {
        expr->_nullable = nullable;
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

// 建议非终结符号大写命名;终结符号小写命名
std::map<std::string, std::pair<std::string, int>> CfUtil::ReadSymbol(Buffer& lexicalBuffer) {
    // key, key_reg_expr, ptiority(数字大，优先级高), is_terminator(1表示是), is_nullable(0,1,2 含义与之前定义一致)
    std::vector<std::string> strVec;
    std::map<std::string, std::pair<std::string, int>> keyRegExprMap;

    while (lexicalBuffer.CurrentCharAvailable()) {
        strVec = lexicalBuffer.GetStringsOfNextLine();

#ifdef DEBUG_CODE
        if (strVec.size() != 5 || this->_symbolMap.find(strVec[0]) != this->_symbolMap.end()) {
            if (strVec.size() != 0) {
                std::cout << "expr lexical line: " << lexicalBuffer._curLine << " error!" << std::endl;
            }
            continue;
        }
#endif

        int priority = std::stoi(strVec[2].c_str());
        int isTerminator = std::stoi(strVec[3].c_str());
        int isNullable = std::stoi(strVec[4].c_str());
        CfSymbol* symbol = AddSymbol(strVec[0], strVec[1], this->_symbolVec.size(), isTerminator == 1, isNullable);
        if (symbol->_isTerminator == 1 && symbol->_nullable == 1) {

#ifdef DEBUG_CODE
            if (this->_nullSymbol != nullptr) {
                std::cout << "null symbol: " << this->_nullSymbol->_key << ", " << symbol->_key << " repeated!" << std::endl;
            }
#endif

            this->_nullSymbol = symbol;
        }
        keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(strVec[0], std::pair<std::string, int>(strVec[1], priority)));
    }
    // _***_格式的符号为保留符号，仅作特殊解析符号
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_END_SYMBOL_", std::pair<std::string, int>("_END_SYMBOL_", 1)));
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_START_SYMBOL_", std::pair<std::string, int>("_START_SYMBOL_", 1)));
    CfSymbol* endSymbol = AddSymbol("_END_SYMBOL_", "_END_SYMBOL_", this->_symbolVec.size(), true, 0);
    CfSymbol* startSymbol = AddSymbol("_START_SYMBOL_", "_START_SYMBOL_", this->_symbolVec.size(), false, 2);

    // 文法表达式解析中的特殊符号：（理论上文法表达式不要将, : ;等作为普通符号，该符号可以用相应英文替代, 在代码分析词法中将相应符号对应英文key进行翻译）
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_number_", std::pair<std::string, int>("0|([1-9][0-9]*)", 0)));
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_string_", 
        std::pair<std::string, int>("\"([0-9]|[a-z]|[A-Z]|_|\\s|\\(|\\)|;|=|,|\n|&|$)*\"", 0)));
    return keyRegExprMap;
}

void CfUtil::ReadExpr(Buffer& exprBuffer, LexicalParser& lexicalParser) {
    // 表达式第一个单词
    std::string initkey = exprBuffer.GetNextStringSplitByBlank();

#ifdef DEBUG_CODE
    if (this->_symbolMap.find(initkey) == this->_symbolMap.end()) {
        std::cout << "init key: " << initkey << " not found!" << std::endl;
    }
#endif

    // 第一个表达式生成 _START_SYMBOL_ initKey _END_SYMBOL_ 格式
    this->_initSymbol = this->_symbolMap.find("_START_SYMBOL_")->second;
    
    CfExpr* expr = new CfExpr();
    expr->_sourceSymbol = this->_initSymbol;
    expr->_production.push_back(this->_symbolMap.find(initkey)->second);
    expr->_production.push_back(this->_symbolMap.find("_END_SYMBOL_")->second);
    expr->_number = -1;
    this->_exprs.insert(std::pair<int, CfExpr*>(expr->_number, expr));
    
    SiblingExprs* exprs = new SiblingExprs();
    exprs->_sourceSymbol = expr->_sourceSymbol;
    exprs->_exprs.insert(expr);
    this->_exprMap.insert(std::pair<CfSymbol*, SiblingExprs*>(expr->_sourceSymbol, exprs));

    // 后续表达式解析
    while (exprBuffer.CurrentCharAvailable()) {
        
        CfExpr* expr = new CfExpr();
        while (exprBuffer.CurrentCharAvailable()) {
            int oldPos = exprBuffer._curPos;
            std::string key = lexicalParser.GetNextWord(exprBuffer);
            auto symbolItr = this->_symbolMap.find(key);
            if (symbolItr == this->_symbolMap.end()) {
                std::string value = exprBuffer.GetString(oldPos, exprBuffer._curPos);
                if (key == "_number_") {
                    expr->_number = std::stoi(value);
                } else if (key == "_string_") {
                    GetExprAdditionalInfo(expr, value);
                } else {
                    delete expr;
                    expr = nullptr;
                    break;
                }
            } else {
                if (expr->_sourceSymbol == nullptr) {

#ifdef DEBUG_CODE
                    if (symbolItr->second->_isTerminator == 1) {
                        std::cout << "warning: symbol " << symbolItr->second->_key << " is terminal symbol!" << std::endl;
                    }
#endif

                    expr->_sourceSymbol = symbolItr->second;
                } else {
                    expr->_production.push_back(symbolItr->second);
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
            exprBuffer.GetNextLine();

#ifdef DEBUG_CODE
            std::cout << "expr file line: " << exprBuffer._curLine << " error!" << std::endl;
#endif

            continue;
        }

#ifdef DEBUG_CODE
        auto exprsItr = this->_exprs.find(expr->_number);
        if (exprsItr != this->_exprs.end()) {
            std::cout << "expr number: " << expr->_number << " repeated!" << std::endl;
        }
#endif

        this->_exprs.insert(std::pair<int, CfExpr*>(expr->_number, expr));
        auto exprMapItr = this->_exprMap.find(expr->_sourceSymbol);
        if (exprMapItr != this->_exprMap.end()) {
            exprMapItr->second->_exprs.insert(expr);
        } else {
            SiblingExprs* exprs = new SiblingExprs();
            exprs->_sourceSymbol = expr->_sourceSymbol;
            exprs->_exprs.insert(expr);
            this->_exprMap.insert(std::pair<CfSymbol*, SiblingExprs*>(expr->_sourceSymbol, exprs));
        }
    }
}

void CfUtil::GetExprAdditionalInfo(CfExpr* expr, const std::string& additionalInfo) {
    std::vector<std::string> infos = StringUtil::split(additionalInfo.substr(1, additionalInfo.size() -2), "&");
    for (auto info : infos) {
        std::vector<std::string> basicInfos = StringUtil::split(info, "=");

#ifdef DEBUG_CODE
        if (basicInfos.size() != 2) {
            std::cout << "expr info" << info << " error!" << std::endl;
            continue;
        }
#endif

        std::string keyWord = StringUtil::trim(basicInfos[0]);
        if (keyWord == "reduction_first") {
            std::vector<std::string> symbols = StringUtil::split(basicInfos[1], ",");
            for (auto symbolName : symbols) {
                auto symbolItr = this->_symbolMap.find(StringUtil::trim(symbolName));

#ifdef DEBUG_CODE
                if (symbolItr == this->_symbolMap.end()) {
                    std::cout << "reduction first symbol" << symbolName << " not found!" << std::endl;    
                    continue;
                }
#endif

                expr->_reductionFirst.insert(symbolItr->second);
            }
        } else if (keyWord == "reduction_priority") {
            expr->_reductionPriority = std::stoi(StringUtil::trim(basicInfos[1]));
        } else if (keyWord == "reduction_action") {
            expr->_reductionAction = StringUtil::trim(basicInfos[1]);
        } else {

#ifdef DEBUG_CODE
            std::cout << "expr additional info: " << basicInfos[0] << " error!" << std::endl;
#endif

        }
    }
}

// 词法与语法之间有一个空行
CfUtil::CfUtil(Buffer& lexicalBuffer, Buffer& exprBuffer) {

    LexicalParser lexicalParser = LexicalParser(ReadSymbol(lexicalBuffer));
    ReadExpr(exprBuffer, lexicalParser);
    
}

void CfUtil::GenInfo() {
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

CfExpr* CfUtil::GetExprByExprNumber(int exprNumber) {
    auto itr = this->_exprs.find(exprNumber);
    if (itr == this->_exprs.end()) {
        return nullptr;
    } else {
        return itr->second;
    }
}