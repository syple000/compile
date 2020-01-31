#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <unordered_map>

#include "../observer_pattern/observer_pattern.h"
#include "../buffer/buffer.h"

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
    // reduced first symbols
    std::set<CfSymbol*> _reductionFirst;
    int _reductionPriority = 0;
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

    void GenNullable();

    void GenFirst();

    void GenNext();

    bool GetFirstOfSymbolVec(std::vector<CfSymbol*>& symbols, int index, std::set<CfSymbol*>& firstSet);

    void GenNextOfSymbol(CfSymbol* symbol);

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

    CfExpr* GetExpr(int exprNumber);

};

#endif