#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <list>

#include "../tree_node/tree_node.h"
#include "./variable_type.h"

#ifndef CODE_GENERATION_VARIABLE_SCOPE
#define CODE_GENERATION_VARIABLE_SCOPE

class Scope {
private:
    ScopeNode* _scopeTreeRoot, *_curScope;

    ScopeNode* GetScope(ScopeNode* pscope, const std::string& scopeName);
    Variable* GetVarInScope(const std::string& varName, const std::string& varCategory, ScopeNode* scope);
public:
    Scope(int rootType);
    
    const std::string& GetScopeName(ScopeNode* Scope);
    const std::string& GetCurrentScopeName();
    ScopeNode* GetScopeByScopeNames(const std::list<std::string>& scopeNames);
    ScopeNode* GetScopeByScopeName(const std::string& scopeName);
    Variable* GetVariable(const std::string& varName, const std::string& varCategory, ScopeNode* scope, bool isUpSearched);

    bool AddVariable(const std::string& varName, const std::string& varType, const std::string& value, const std::string& category,
         int openness, int lifeCycle, bool isPointer);
    bool AddVariableType(const std::string& typeName, int openness);
    ScopeNode* AddScope(const std::string& scopeName, int type);
    bool AddPlaceholderScope(const std::string& scopeName);

    // 回收内存使用空间: 大型项目需要
    bool RemoveScope(ScopeNode* scope);

    bool BacktrackScope();
    ScopeNode* TransScope(ScopeNode* scope);
    ScopeNode* TransScopeByName(const std::string& scopeName);
};

#endif