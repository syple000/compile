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
                    expr->_production[i]->_positionInExpr.push_back(std::pair<CfExpr*, int>(expr, i));
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

    delete this->_paramParser;
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
    std::string matchStr = "[0-9]|[a-z]|[A-Z]|_|\\s|\\(|\\)|;|=|,|\n|&|$|\\\\|<|>|:|-|+|\\*|/|\\.|\"|#|\\[|\\]|?|!|\\|";
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_code_block_",
        std::pair<std::string, int>("%%(" + matchStr + "|{|})*%%", 0)));
    // 支持{}嵌套，最大三层
    for (int i = 0; i < 3; i++) {
        matchStr = "(({(" + matchStr + ")*})|(" + matchStr + ")*)*";
    }
    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_string_", 
        std::pair<std::string, int>("{" + matchStr + "}", 0)));

    keyRegExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("_semicolon_", std::pair<std::string, int>(";", 0)));
    return keyRegExprMap;
}

void CfUtil::AddInitExpr(const std::string& initKey) {
    // 第一个表达式生成 _START_SYMBOL_ initKey _END_SYMBOL_ 格式
    this->_initSymbol = this->_symbolMap.find("_START_SYMBOL_")->second;
    
    CfExpr* expr = new CfExpr();
    expr->_sourceSymbol = this->_initSymbol;
    expr->_production.push_back(this->_symbolMap.find(initKey)->second);
    expr->_production.push_back(this->_symbolMap.find("_END_SYMBOL_")->second);
    expr->_number = -1;
    this->_exprs.insert(std::pair<int, CfExpr*>(expr->_number, expr));
    
    SiblingExprs* exprs = new SiblingExprs();
    exprs->_sourceSymbol = expr->_sourceSymbol;
    exprs->_exprs.insert(expr);
    this->_exprMap.insert(std::pair<CfSymbol*, SiblingExprs*>(expr->_sourceSymbol, exprs));

}

void CfUtil::AddExprs(Buffer& exprBuffer, LexicalParser& lexicalParser, Buffer& auxiliaryCodeBuffer, std::vector<std::string>& funcNames) {
    // 表达式解析
    while (exprBuffer.CurrentCharAvailable()) {
        
        CfExpr* expr = new CfExpr();
        while (exprBuffer.CurrentCharAvailable()) {
            int oldPos = exprBuffer._curPos;
            std::string key = lexicalParser.GetNextWord(exprBuffer);
            auto symbolItr = this->_symbolMap.find(key);
            if (symbolItr == this->_symbolMap.end()) {
                std::string value = StringUtil::Trim(exprBuffer.GetString(oldPos, exprBuffer._curPos));
                if (key == "_number_") {
                    expr->_number = std::stoi(value);
                } else if (key == "_string_") {
                    GetExprAdditionalInfo(expr, value, auxiliaryCodeBuffer, funcNames);
                } else if (key == "_semicolon_") {
                    // 单独分号匹配作为表达式结束标志
                    while (exprBuffer.CurrentCharAvailable() && (exprBuffer.GetCurrentChar() == ' ' || exprBuffer.GetCurrentChar() == '\n')) {
                        exprBuffer.MoveOnByChar();
                    }
                    break;
                } else {

#ifdef DEBUG_CODE
                    delete expr;
                    expr = nullptr;
                    std::cout << "expr file line: " << exprBuffer._curLine << " error!" << std::endl;
                    break;
#endif

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
        }

#ifdef DEBUG_CODE
        if (expr == nullptr) {
            break;
        }
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

void CfUtil::ReadExpr(Buffer& exprBuffer, LexicalParser& lexicalParser) {
    Buffer auxiliaryCodeBuf(100);

    // 表达式第一个单词
    std::string initkey = exprBuffer.GetNextStringSplitByBlank();

#ifdef DEBUG_CODE
    if (this->_symbolMap.find(initkey) == this->_symbolMap.end()) {
        std::cout << "init key: " << initkey << " not found!" << std::endl;
        return;
    }
#endif
    
    AddInitExpr(initkey);

    // 读取辅助代码块
    int oldPos = exprBuffer._curPos;
    std::string auxiliaryCodeKey = lexicalParser.GetNextWord(exprBuffer);
    std::string auxiliaryCode = StringUtil::Trim(exprBuffer.GetString(oldPos, exprBuffer._curPos));

#ifdef DEBUG_CODE
    if (auxiliaryCodeKey != "_code_block_") {
        std::cout << "auxiliary code format error!" << std::endl;
    }
#endif

    // 读取表达式并处理归约动作的代码生成
    auxiliaryCodeBuf.AppendToBuffer(5, "#define GEN_AUX_CODE_FILE 1\n#ifndef AUX_CODE\n#define AUX_CODE 1\n\n",
        "#include \"../cf_analysis_info.h\"\n", auxiliaryCode.substr(2, auxiliaryCode.size() - 4).c_str(),
        "\nclass AuxCode {\npublic:\n", "\n    std::unordered_map<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)> funcRegistry;\n");

    std::vector<std::string> funcNames;
    // 读取表达式
    AddExprs(exprBuffer, lexicalParser, auxiliaryCodeBuf, funcNames);

    auxiliaryCodeBuf.AppendToBuffer("\n    void registFuncs() {\n");
    for (auto func : funcNames) {
        auxiliaryCodeBuf.AppendToBuffer(5, "        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>(\"",
            func.c_str() ,"\", ", func.c_str(), "));\n");
    }
    auxiliaryCodeBuf.AppendToBuffer(2, "    }\n};\n", "#endif\n");

    IO<std::string> io(String2String, String2String);
    io.WriteFile(auxiliaryCodeBuf, "./context_free/aux_code/aux_code.h", std::ios::binary);
}

void CfUtil::GetExprAdditionalInfo(CfExpr* expr, const std::string& additionalInfo, Buffer& auxiliaryCodeBuffer, std::vector<std::string>& funcNames) {
    std::string info = additionalInfo.substr(1, additionalInfo.size() -2);
    if (StringUtil::StartWith(info, "reduction_first=")) {
        std::vector<std::string> symbols = StringUtil::Split(info.substr(16, info.size() - 16), this->_commaRegExprEngine);
        for (auto symbolName : symbols) {
            auto symbolItr = this->_symbolMap.find(StringUtil::Trim(symbolName));

#ifdef DEBUG_CODE
            if (symbolItr == this->_symbolMap.end()) {
                std::cout << "reduction first symbol" << symbolName << " not found!" << std::endl;    
                continue;
            }
#endif

            expr->_reductionFirst.insert(symbolItr->second);
        }
    } else if (StringUtil::StartWith(info, "reduction_priority=")) {
        expr->_reductionPriority = std::stoi(StringUtil::Trim(info.substr(19, info.size() - 19)));
    } else if (StringUtil::StartWith(info, "reduction_action=") || StringUtil::StartWith(info, "action_")) {
        std::string action;
        std::string funcName;
        if (StringUtil::StartWith(info, "reduction_action=")) {
            funcName = GetFuncName(std::to_string(expr->_number), true);
            action = info.substr(17, info.size() - 17);
            expr->_actions.insert(std::pair<int, std::string>(-1, funcName));
        } else {
            std::string funcStr;
            int i = 7;
            for (; i < info.size(); i++) {
                if (info[i] == '=') {
                    break;
                }
                funcStr += info[i];
            }
            funcName = GetFuncName(StringUtil::Trim(funcStr), false);
            action = info.substr(i + 1, info.size() - i - 1);
            expr->_actions.insert(std::pair<int, std::string>(expr->_production.size(), funcName));
        }
        funcNames.push_back(funcName);
        // 生成归约动作代码
        auxiliaryCodeBuffer.AppendToBuffer(5, "    static void ", funcName.c_str(), "(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {\n", 
            StringUtil::Replace(action, *this->_paramParser).c_str(), "\n    }\n");
    } else {

#ifdef DEBUG_CODE
        std::cout << "expr additional info: " << additionalInfo << " error!" << std::endl;
#endif

    }
}

// 词法与语法之间有一个空行
CfUtil::CfUtil(Buffer& lexicalBuffer, Buffer& exprBuffer) : _commaRegExprEngine(",") {
    std::map<std::string, std::pair<std::string, int>> keyRegMap;
    keyRegMap.insert(std::pair<std::string, std::pair<std::string, int>>("pinfo", std::pair<std::string, int>("$0", 0)));
    keyRegMap.insert(std::pair<std::string, std::pair<std::string, int>>("kinfos", std::pair<std::string, int>("$1", 0)));
    // 以下为快速元素定位，非必须
    for (int i = 1; i < 10; i++) {
        std::string numStr = std::to_string(i);
        std::string key = "kinfos[kinfos.size() - " + numStr + "]";
        std::string keyReg = "$-" + numStr; 
        keyRegMap.insert(std::pair<std::string, std::pair<std::string, int>>(key, std::pair<std::string, int>(keyReg, 0)));
    }
    this->_paramParser = new LexicalParser(keyRegMap);

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

std::string CfUtil::GetFuncName(const std::string& func, bool isReductionFunc) {
    if (isReductionFunc) {
        return "_func_reduction_" + func + "_";
    } else {
        return "_func_" + func + "_";
    }
}