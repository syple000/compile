
#include "./variable_scope.h"

Scope::Scope(int rootType) {
    this->_scopeTreeRoot = new ScopeNode(nullptr, "", rootType);
    this->_curScope = this->_scopeTreeRoot;
}

const std::string& Scope::GetScopeName(ScopeNode* Scope) {
    return Scope->_scopeName;
}

const std::string& Scope::GetCurrentScopeName() {
    return GetScopeName(this->_curScope);
}

bool Scope::AddVariable(const std::string& varName, const std::string& varType, const std::string& value, const std::string& category, 
    int openness, int lifeCycle) {
    VariableType* type = this->_curScope->GetVariableType(varType);
    if (type == nullptr) {
        return false;
    }
    return this->_curScope->AddVariable(type, varName, value, openness, lifeCycle, category);
}

bool Scope::AddVariableType(const std::string& typeName, int openness) {
    return this->_curScope->AddVariableType(typeName, openness);
}

ScopeNode* Scope::AddScope(const std::string& scopeName, int type) {
    return this->_curScope->AddChildScope(scopeName, type);
}

bool Scope::AddPlaceholderScope(const std::string& scopeName) {
    return this->_curScope->AddPlaceholderChildScope(scopeName);
}

bool Scope::RemoveScope(ScopeNode* scope) {
    ScopeNode* pscope = scope->_pnode;
    if (pscope != nullptr) {
        ScopeNode* removeScope = pscope->RemoveChildScope(scope->_scopeName);
        if (removeScope == nullptr) {
            return false;
        }
        ScopeNode::DestroyTree(removeScope);
    } else {
        ScopeNode::DestroyTree(scope);
    }
    return true;
}

bool Scope::BacktrackScope() {
    if (this->_curScope->_pnode == nullptr) {
        return false;
    }
    this->_curScope = this->_curScope->_pnode;
    return true;
}

ScopeNode* Scope::TransScope(ScopeNode* scope) {
    this->_curScope = scope;
    return scope;
}

ScopeNode* Scope::TransScopeByName(const std::string& scopeName) {
    ScopeNode* nextScope = GetScope(this->_curScope, scopeName);
    if (nextScope != nullptr) {
        TransScope(nextScope);
    }
    return nextScope;
}

ScopeNode* Scope::GetScopeByScopeNames(const std::list<std::string>& scopeNames) {
    ScopeNode* root = this->_scopeTreeRoot;
    for (auto itr : scopeNames) {
        root = GetScope(root, itr);
        if (root == nullptr) {
            break;
        }
    }
    return root;
}

ScopeNode* Scope::GetScopeByScopeName(const std::string& scopeName) {
    return GetScope(this->_curScope, scopeName);
}

// 获取后需要进行变量可见性分析
Variable* Scope::GetVariable(const std::string& varName, const std::string& varCategory, ScopeNode* scope, bool isUpSearched) {
    Variable* var = GetVarInScope(varName, varCategory, scope);
    if (var == nullptr && isUpSearched) {
        ScopeNode* curScope = scope->_pnode;
        while (curScope != nullptr) {
            if (curScope->_type == 0) {
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

ScopeNode* Scope::GetScope(ScopeNode* pscope, const std::string& scopeName) {
    auto itr = pscope->_cnodes.find(scopeName);
    if (itr == pscope->_cnodes.end()) {
        return nullptr;
    } else {
        return itr->second;
    }
}

Variable* Scope::GetVarInScope(const std::string& varName, const std::string& varCategory, ScopeNode* scope) {
    auto varsItr = scope->_vars.find(varCategory);
    if (varsItr == scope->_vars.end()) {
        return nullptr;
    }
    auto varItr = varsItr->second.find(varName);
    if (varItr != varsItr->second.end()) {
        return varItr->second;
    } else {
        return nullptr;
    }
}