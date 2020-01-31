#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <iostream>

#include "../code_generation/variable.h"
#include "../code_generation/variable_type.h"

#define DEBUG_CODE 1

#ifndef TREE_NODE
#define TREE_NODE 1

struct CfTreeNode {
    CfTreeNode* _pnode = nullptr;
    std::vector<CfTreeNode*> _cnodes;
    std::string _key;
    std::string _value;
    // generated code
    std::string _code;

    CfTreeNode(const std::string& key, const std::string& value) : _key(key), _value(value) {}

    CfTreeNode(const std::string& key, const std::vector<CfTreeNode*>& cnodes) 
        : _cnodes(cnodes), _key(key) {
        for (int i = 0; i < this->_cnodes.size(); i++) {
            this->_cnodes[i]->_pnode = this;
        }
    }

    static void DestroyTree(CfTreeNode* root) {
        if (root == nullptr) {
            return;
        }
        for (int i = 0; i < root->_cnodes.size(); i++) {
            DestroyTree(root->_cnodes[i]);
        }
        delete root;
    }

    static void TraverseTree(CfTreeNode* root, void(*func)(CfTreeNode*)) {
        if (root == nullptr) {
            return;
        }
        for (int i = 0; i < root->_cnodes.size(); i++) {
            TraverseTree(root->_cnodes[i], func);
            func(root->_cnodes[i]);
        }
    }
};

struct RegExprNode {
    unsigned char _content;
    bool _nullable;
    RegExprNode* _leftChildNode, * _rightChildNode;
    std::set<RegExprNode*> *_first = nullptr, *_last = nullptr, *_next = nullptr;

    RegExprNode(char content, RegExprNode* leftChildNode, RegExprNode* rightChildNode, bool nullable) 
    : _content(content), _leftChildNode(leftChildNode), _rightChildNode(rightChildNode), _nullable(nullable) {
        _first = new std::set<RegExprNode*>();
        _last = new std::set<RegExprNode*>();
        // init first and last
        if (_leftChildNode == nullptr && _rightChildNode == nullptr) {    
            _first->insert(this);
            _last->insert(this);
            _next = new std::set<RegExprNode*>();
        }
    }

    RegExprNode() = default;
    
    virtual ~RegExprNode() {
        delete _first;
        delete _last;
        delete _next;
    }

    static void DestroyTree(RegExprNode* root) {
        if (root == nullptr) {
            return;
        }
        DestroyTree(root->_leftChildNode);
        DestroyTree(root->_rightChildNode);
        delete root;
    }
};

struct ScopeNode {
    ScopeNode* _pnode;
    std::unordered_map<std::string, ScopeNode*> _cnodes;

    std::string _scopeName;
    std::unordered_map<std::string, VariableType*> _variableTypes;
    // 同一类型下不可以重复命名
    std::vector<std::unordered_map<std::string, Variable*>> _declVars;
    // scope的类型
    int _type;
    // 子scope可见当前scope中变量的 类型集合
    std::set<int> _childScopeVisibleTypes;
    int _anonymousCount = 0;

    ScopeNode(ScopeNode* pnode, const std::string& scopeName, int categoryCount, int type, const std::set<int>& childScopeVisibleTypes) 
        : _pnode(pnode), _scopeName(scopeName), _declVars(categoryCount), _type(type), _childScopeVisibleTypes(childScopeVisibleTypes) {
        auto scopeItr = this->_pnode->_cnodes.find(scopeName);
        if (scopeItr == this->_pnode->_cnodes.end()) {
            this->_pnode->_cnodes.insert(std::pair<std::string, ScopeNode*>(scopeName, this));
        } else {
            scopeItr->second = this;
        }
    }

    bool AddDeclVariable(Variable* var, int category) {
        if (this->_declVars[category].find(var->_name) != this->_declVars[category].end()) {
            return false;
        }
        this->_declVars[category].insert(std::pair<std::string, Variable*>(var->_name, var));
        return true;
    }

    bool AddVariableType(VariableType* varType) {
        if (this->_variableTypes.find(varType->_name) != this->_variableTypes.end()) {
            return false;
        }
        this->_variableTypes.insert(std::pair<std::string, VariableType*>(varType->_name, varType));
        return true;
    }

    ScopeNode* GetChildScopeNode(const std::string& scopeName) {
        auto itr = this->_cnodes.find(scopeName);
        if (itr == this->_cnodes.end()) {
            return nullptr;
        } else {
            return itr->second;
        }
    }

    VariableType* GetVariableType(const std::string& typeName) {
        auto itr = this->_variableTypes.find(typeName);
        if (itr == this->_variableTypes.end()) {
            return nullptr;
        } else {
            return itr->second;
        }
    }

    Variable* GetVariable(const std::string& varName, int varCategory) {
        auto itr = this->_declVars[varCategory].find(varName);
        if (itr == this->_declVars[varCategory].end()) {
            return nullptr;
        } else {
            return itr->second;
        }
    }

    ScopeNode* AddChildScope(const std::string& scopeName, int type, const std::set<int>& childScopeVisibleTypes) {
        std::string scopeNameToAdd = scopeName;
        if (scopeName == "") {
            scopeNameToAdd += std::to_string(this->_anonymousCount++);
        }
        auto itr = this->_cnodes.find(scopeName);
        if (itr == this->_cnodes.end() || itr->second == nullptr) {
            return new ScopeNode(this, scopeNameToAdd, this->_declVars.size(), type, childScopeVisibleTypes);   
        }
        return nullptr;
    }

    // scope name 不为""
    bool AddPlaceholderChildScope(const std::string& scopeName) {
        auto itr = this->_cnodes.find(scopeName);
        if (itr == this->_cnodes.end()) {
            this->_cnodes.insert(std::pair<std::string, ScopeNode*>(scopeName, nullptr));
            return true;
        } else {
            return false;
        }
    }

    ScopeNode* RemoveChildScope(const std::string& scopeName) {
        auto itr = this->_cnodes.find(scopeName);
        if (itr == this->_cnodes.end()) {
            return nullptr;
        }
        ScopeNode* removeScope = itr->second;
        this->_cnodes.erase(itr);
        removeScope->_pnode = nullptr;
        return removeScope;
    }

    virtual ~ScopeNode() {
        for (auto vars : this->_declVars) {
            for (auto itr : vars) {
                delete itr.second;
            }
        }
        for (auto varType : this->_variableTypes) {
            delete varType.second;
        }
    }

    static void DestroyTree(ScopeNode* root) {
        if (root == nullptr) {
            return;
        }
        for (auto node : root->_cnodes) {
            DestroyTree(node.second);
        }
        delete root;
    }
};

#endif