#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <iostream>

#include "../code_analysis/variable.h"
#include "../code_analysis/variable_type.h"

#define DEBUG_CODE 1

#ifndef TREE_NODE
#define TREE_NODE 1

struct CfTreeNode {
    CfTreeNode* _pnode = nullptr;
    std::vector<CfTreeNode*> _cnodes;
    std::string _key;
    std::string _value;
    int _reducedExprNumber;
    // 如果节点被上层代码生成依赖，则将代码暂存于该变量
    std::string _code;

    CfTreeNode(const std::string& key, const std::string& value) : _key(key), _value(value), _reducedExprNumber(-1) {}

    // reduced expr number用于确定代码生成分析时确定归约的表达式与归约方法
    CfTreeNode(const std::string& key, const std::vector<CfTreeNode*>& cnodes, int reducedExprNumber) 
        : _cnodes(cnodes), _key(key), _reducedExprNumber(reducedExprNumber) {
        for (int i = 0; i < this->_cnodes.size(); i++) {
            this->_cnodes[i]->_pnode = this;
        }
    }

    int GetPosInChildNodes() {
        if (this->_pnode == nullptr) {
            return -1;
        }
        for (int i = 0; i < this->_pnode->_cnodes.size(); i++) {
            if (this->_pnode->_cnodes[i] == this) {
                return i;
            }
        }

#ifdef DEBUG_CODE
        std::cout << "cf tree node: " << this->_key << " pos error!" << std::endl;
#endif

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
        }
        func(root);
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
    std::unordered_map<std::string, std::unordered_map<std::string, Variable*>> _vars;
    // 0 子作用域可以直接查询变量; >0 需要引用作用域查询
    int _type;
    int _anonymousCount = 0;

    ScopeNode(ScopeNode* pnode, const std::string& scopeName, int type) 
        : _pnode(pnode), _scopeName(scopeName), _type(type) {
        if (this->_pnode != nullptr) {
            auto scopeItr = this->_pnode->_cnodes.find(scopeName);
            if (scopeItr == this->_pnode->_cnodes.end()) {
                this->_pnode->_cnodes.insert(std::pair<std::string, ScopeNode*>(scopeName, this));
            } else if (scopeItr->second == nullptr) {
                scopeItr->second = this;
            } else {

#ifdef DEBUG_CODE
                std::cout << "pnode: " << pnode->_scopeName << ", cnode: " << scopeName << " repeated!" << std::endl;
#endif

            }
        }
    }

    bool AddVariable(VariableType* type, const std::string& varName, const std::string& value, int openness, int lifeCycle, 
        const std::string& varCategory, bool isPointer) {
        if (this->_vars.find(varCategory) == this->_vars.end()) {
            this->_vars.insert(std::pair<std::string, std::unordered_map<std::string, Variable*>>(varCategory, std::unordered_map<std::string, Variable*>()));
        }
        auto vars = this->_vars.find(varCategory);
        if (vars->second.find(varName) != vars->second.end()) {
            return false;
        }
        vars->second.insert(std::pair<std::string, Variable*>(varName, new Variable(type, varName, value, openness, lifeCycle, isPointer)));
        return true;
    }

    bool AddVariableType(const std::string& typeName, int openness) {
        if (this->_variableTypes.find(typeName) != this->_variableTypes.end()) {
            return false;
        }
        this->_variableTypes.insert(std::pair<std::string, VariableType*>(typeName, new VariableType(typeName, this, openness)));
        return true;
    }

    ScopeNode* AddChildScope(const std::string& scopeName, int type) {
        std::string scopeNameToAdd = scopeName;
        if (scopeName == "") {
            scopeNameToAdd += std::to_string(this->_anonymousCount++);
        }
        auto itr = this->_cnodes.find(scopeName);
        if (itr == this->_cnodes.end() || itr->second == nullptr) {
            return new ScopeNode(this, scopeNameToAdd, type);   
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

    ScopeNode* GetChildScope(const std::string& scopeName) {
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

    Variable* GetVariable(const std::string& varName, const std::string& varCategory) {
        auto varsItr = this->_vars.find(varCategory);
        auto itr = varsItr->second.find(varName);
        if (itr == varsItr->second.end()) {
            return nullptr;
        } else {
            return itr->second;
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
        for (auto vars : this->_vars) {
            for (auto itr : vars.second) {
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