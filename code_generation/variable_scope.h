#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <list>

#include "../tree_node/tree_node.h"
#include "./variable_type.h"

#ifndef CODE_GENERATION_VARIABLE_SCOPE
#define CODE_GENERATION_VARIABLE_SCOPE

class VarScope {
private:
    // 以解析后的key作为匹配标准
    // 作用域左侧与右侧
    std::string _leftStart;
    std::string _rightEnd;
    // 变量的key
    std::string _varKey;

    ScopeNode* _scopeTreeRoot, *_curScope;

    ScopeNode* GetScope(ScopeNode* pscope, const std::string& scopeName) {
        auto itr = pscope->_cnodes.find(scopeName);
        if (itr == pscope->_cnodes.end()) {
            return nullptr;
        } else {
            return itr->second;
        }
    }

    Variable* GetVarInScope(const std::string& varName, int varCategory, ScopeNode* scope) {
        auto varItr = scope->_declVars[varCategory].find(varName);
        if (varItr == scope->_declVars[varCategory].end()) {
            return varItr->second;
        } else {
            return nullptr;
        }
    }
public:
    VarScope(const std::string& leftStart, const std::string& rightEnd, const std::string& varKey, int varCategory, 
        int rootType, const std::set<int>& rootVisibleTypes) 
        : _leftStart(leftStart), _rightEnd(rightEnd), _varKey(varKey) {
        this->_scopeTreeRoot = new ScopeNode(nullptr, "", varCategory, rootType, rootVisibleTypes);
        this->_curScope = this->_scopeTreeRoot;
    }

    int GetKeyType(const std::string& key) {
        if (key == this->_varKey) {
            return 0;
        } else if (key == this->_leftStart) {
            return 1;
        } else if (key == this->_rightEnd) {
            return 2;
        } else {
            return -1;
        }
    }

    std::string& GetScopeName(ScopeNode* varScope) {
        return varScope->_scopeName;
    }

    std::string& GetCurrentScopeName() {
        return GetScopeName(this->_curScope);
    }

    bool AddImmediateVar(const std::string& varName, const std::string& varType, const std::string& immediate, int category, int openness, int lifeCycle) {
        VariableType* type = this->_curScope->GetVariableType(varType);
        if (type == nullptr) {
            return false;
        }
        Variable* var = new Variable(type, varName, immediate, openness, lifeCycle);
        bool success = this->_curScope->AddDeclVariable(var, category);
        if (!success) {
            delete var;
        }
        return success;
    }

    bool AddReferenceVar(const std::string& varName, const std::string& varType, int reference, int category, int openness, int lifeCycle) {
        VariableType* type = this->_curScope->GetVariableType(varType);
        if (type == nullptr) {
            return false;
        }
        Variable* var = new Variable(type, varName, reference, openness, lifeCycle);
        bool success = this->_curScope->AddDeclVariable(var, category);
        if (!success) {
            delete var;
        }
        return success;
    }

    bool AddVariableType(const std::string& typeName, int openness) {
        VariableType* type = new VariableType(typeName, this->_curScope, openness);
        bool success = this->_curScope->AddVariableType(type);
        if (!success) {
            delete type;
        }
        return success;
    }

    ScopeNode* AddScope(const std::string& scopeName, int type, const std::set<int>& childScopeVisibleTypes) {
        return this->_curScope->AddChildScope(scopeName, type, childScopeVisibleTypes);
    }

    void BacktrackScope() {
        this->_curScope = this->_curScope->_pnode;
    }

    ScopeNode* TransScope(ScopeNode* scope) {
        this->_curScope = scope;
    }

    ScopeNode* TransScopeByName(const std::string& scopeName) {
        ScopeNode* nextScope = GetScope(this->_curScope, scopeName);
        if (nextScope != nullptr) {
            TransScope(nextScope);
        }
        return nextScope;
    }

    ScopeNode* GetScopeByScopeNames(const std::list<std::string>& scopeNames) {
        ScopeNode* root = this->_scopeTreeRoot;
        for (auto itr : scopeNames) {
            root = GetScope(root, itr);
            if (root == nullptr) {
                break;
            }
        }
        return root;
    }

    ScopeNode* GetScopeByScopeName(const std::string& scopeName) {
        return GetScope(this->_curScope, scopeName);
    }

    // 获取后需要进行变量可见性分析
    Variable* GetVariable(const std::string& varName, int varCategory, ScopeNode* scope, bool fromParentScope) {
        Variable* var = GetVarInScope(varName, varCategory, scope);
        if (var == nullptr && fromParentScope) {
            int scopeType = scope->_type;
            ScopeNode* curScope = scope->_pnode;
            while (curScope != nullptr) {
                if (curScope->_childScopeVisibleTypes.find(scopeType) != curScope->_childScopeVisibleTypes.end()) {
                    var = GetVarInScope(varName, varCategory, scope);
                    if (var != nullptr) {
                        break;
                    }
                }
                curScope = curScope->_pnode;
            }
        }
        return var;
    }
};

#endif