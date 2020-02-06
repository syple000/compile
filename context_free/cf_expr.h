#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <unordered_map>

#include "../observer_pattern/observer_pattern.h"
#include "../buffer/buffer.h"
#include "./lexical_parser.h"
#include "../io/io.h"
#include "../io/format_conversion.h"

#ifndef CONTEXT_FREE
#define CONTEXT_FREE 1

struct CfExpr;
struct CfSymbol;
struct SiblingExprs;
struct SymbolSubject;

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

    CfSymbol(const std::string& key, const std::string& keyRegExpr,  int number, bool isTerminator, int nullable);

    virtual ~CfSymbol();

};

struct SymbolSubject {
    static bool NullableInfoObserverUpdate(CfSymbol* const &subjectSymbol, SiblingExprs* &observerExprs);

    static bool FirstInfoObserverUpdate(CfSymbol* const& subjectSymbol, CfSymbol* &observerSymbol);

    static bool NextInfoObserverUpdate(CfSymbol* const& subjectSymbol, CfSymbol* &observerSymbol);

    Subject<CfSymbol*, SiblingExprs*> _nullableInfoSubject;
    Subject<CfSymbol*, CfSymbol*> _firstInfoSubject;
    Subject<CfSymbol*, CfSymbol*> _nextInfoSubject;

    SymbolSubject(CfSymbol* symbol);
};

struct CfExpr {
    CfSymbol* _sourceSymbol = nullptr;
    std::vector<CfSymbol*> _production;
    int _nullable = 2;
    // 归约约定
    std::set<CfSymbol*> _reductionFirst;
    int _reductionPriority = 0;
    std::string _reductionAction;
    // expression identifier
    int _number = -1;
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
    std::vector<CfSymbol*> _symbolVec;
    std::unordered_map<int, CfExpr*> _exprs;
    CfSymbol* _initSymbol = nullptr;
    CfSymbol* _nullSymbol = nullptr;

    // 正则解析器
    RegExprEngine _commaRegExprEngine;
    // 归约动作符号替换(目前支持0-9号变量替换，可根据需要扩展)
    LexicalParser* _paramParser;

    void GenNullable();

    void GenFirst();

    void GenNext();

    bool GetFirstOfSymbolVec(std::vector<CfSymbol*>& symbols, int index, std::set<CfSymbol*>& firstSet);

    void GenNextOfSymbol(CfSymbol* symbol);

    std::map<std::string, std::pair<std::string, int>> ReadSymbol(Buffer& lexicalBuffer);

    void ReadExpr(Buffer& exprBuffer, LexicalParser& lexicalParser);

    void GetExprAdditionalInfo(CfExpr* expr, const std::string& additionalInfo, Buffer& auxiliaryCodeBuffer, std::vector<std::string>& funcNames);

    void AddInitExpr(const std::string& initKey);

    void AddExprs(Buffer& exprBuffer, LexicalParser& lexicalParser, Buffer& auxiliaryCodeBuffer, std::vector<std::string>& funcNames);

    CfSymbol* AddSymbol(const std::string& key, const std::string& keyRegExpr, int number, bool isTerminator, int nullable);

public:

    CfUtil(Buffer& lexicalBuffer, Buffer& exprBuffer);

    virtual ~CfUtil();

    static int IsExprNullable(CfExpr* expr, std::set<CfSymbol*>* dependings);

    static int IsSymbolNullable(CfSymbol* symbol, SiblingExprs* exprs);

    static bool FirstOfSymbol(CfSymbol* symbol, SiblingExprs* exprs);

    static bool FirstOfExpr(CfExpr* expr, std::set<CfSymbol*>* dependings);

    void GenInfo();

    CfSymbol* GetCfSymbol(const std::string& symbolKey);

    SiblingExprs* GetSiblingExprs(CfSymbol* symbol);

    CfSymbol* GetInitSymbol();

    CfSymbol* GetNullSymbol();

    CfSymbol* GetSymbolByIndex(int index);

    int GetSymbolCount();

    CfExpr* GetExprByExprNumber(int exprNumber);

    static std::string GetReductionFuncName(int exprNumber);

};

#endif