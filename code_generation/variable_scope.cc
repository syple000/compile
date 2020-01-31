
#include "./variable_scope.h"

Scope::Scope(const std::unordered_map<std::string, int>& keyTypeMap, int varCategory, 
    int rootType, const std::set<int>& rootVisibleTypes) : _keyTypeMap(keyTypeMap) {
    this->_scopeTreeRoot = new ScopeNode(nullptr, "", varCategory, rootType, rootVisibleTypes);
    this->_curScope = this->_scopeTreeRoot;
}

int Scope::GetKeyType(const std::string& key) {
    auto keyTypeItr = this->_keyTypeMap.find(key);
    if (keyTypeItr == this->_keyTypeMap.end()) {
        return -1;
    } else {
        return keyTypeItr->second;
    }
}

std::string& Scope::GetScopeName(ScopeNode* Scope) {
    return Scope->_scopeName;
}

std::string& Scope::GetCurrentScopeName() {
    return GetScopeName(this->_curScope);
}

bool Scope::AddImmediateVar(const std::string& varName, const std::string& varType, const std::string& immediate, int category, int openness, int lifeCycle) {
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

bool Scope::AddReferenceVar(const std::string& varName, const std::string& varType, int reference, int category, int openness, int lifeCycle) {
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

bool Scope::AddVariableType(const std::string& typeName, int openness) {
    VariableType* type = new VariableType(typeName, this->_curScope, openness);
    bool success = this->_curScope->AddVariableType(type);
    if (!success) {
        delete type;
    }
    return success;
}

ScopeNode* Scope::AddScope(const std::string& scopeName, int type, const std::set<int>& childScopeVisibleTypes) {
    return this->_curScope->AddChildScope(scopeName, type, childScopeVisibleTypes);
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

void Scope::BacktrackScope() {
    this->_curScope = this->_curScope->_pnode;
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
Variable* Scope::GetVariable(const std::string& varName, int varCategory, ScopeNode* scope, bool fromParentScope) {
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

ScopeNode* Scope::GetScope(ScopeNode* pscope, const std::string& scopeName) {
    auto itr = pscope->_cnodes.find(scopeName);
    if (itr == pscope->_cnodes.end()) {
        return nullptr;
    } else {
        return itr->second;
    }
}

Variable* Scope::GetVarInScope(const std::string& varName, int varCategory, ScopeNode* scope) {
    auto varItr = scope->_declVars[varCategory].find(varName);
    if (varItr == scope->_declVars[varCategory].end()) {
        return varItr->second;
    } else {
        return nullptr;
    }
}