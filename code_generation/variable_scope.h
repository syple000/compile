#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <list>

#include "../tree_node/tree_node.h"
#include "./variable_type.h"

#ifndef CODE_GENERATION_VARIABLE_SCOPE
#define CODE_GENERATION_VARIABLE_SCOPE

/*  变量类型，函数名，普通变量名等数据作用域
*/
class Scope {
private:

    std::unordered_map<std::string, int> _keyTypeMap;
    ScopeNode* _scopeTreeRoot, *_curScope;

    ScopeNode* GetScope(ScopeNode* pscope, const std::string& scopeName);
    Variable* GetVarInScope(const std::string& varName, int varCategory, ScopeNode* scope);
public:
    Scope(const std::unordered_map<std::string, int>& keyTypeMap, int varCategory, int rootType, const std::set<int>& rootVisibleTypes);
    
    int GetKeyType(const std::string& key);
    
    std::string& GetScopeName(ScopeNode* Scope);
    std::string& GetCurrentScopeName();
    
    bool AddImmediateVar(const std::string& varName, const std::string& varType, const std::string& immediate, int category, int openness, int lifeCycle);
    bool AddReferenceVar(const std::string& varName, const std::string& varType, int reference, int category, int openness, int lifeCycle);
    
    bool AddVariableType(const std::string& typeName, int openness);
    
    ScopeNode* AddScope(const std::string& scopeName, int type, const std::set<int>& childScopeVisibleTypes);
    bool AddPlaceholderScope(const std::string& scopeName);
    // 回收内存使用空间: 大型项目需要
    bool RemoveScope(ScopeNode* scope);

    void BacktrackScope();
    ScopeNode* TransScope(ScopeNode* scope);
    ScopeNode* TransScopeByName(const std::string& scopeName);
    
    ScopeNode* GetScopeByScopeNames(const std::list<std::string>& scopeNames);
    ScopeNode* GetScopeByScopeName(const std::string& scopeName);
    
    Variable* GetVariable(const std::string& varName, int varCategory, ScopeNode* scope, bool fromParentScope);
};

#endif