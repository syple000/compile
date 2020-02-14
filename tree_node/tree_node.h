#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <iostream>

#define DEBUG_CODE 1

#ifndef TREE_NODE
#define TREE_NODE 1

struct CfTreeNode {

    // 属性都必须继承该类
    struct Attribute {
        std::string _name;
        Attribute() {}
        Attribute(const std::string& name) : _name(name) {}
        virtual ~Attribute() {}
    };

    CfTreeNode* _pnode = nullptr;
    std::vector<CfTreeNode*> _cnodes;
    std::string _key;
    std::string _value;
    int _reducedExprNumber;
    // 节点在字节点中的位置
    int _index;
    std::string _action;
    std::string _reductionAction;
    std::unordered_map<std::string, Attribute*> _attributes;

    CfTreeNode(const std::string& key, const std::string& value, const std::string& action) 
        : _key(key), _value(value), _reducedExprNumber(-1), _index(-1), _action(action) {}

    // reduced expr number用于确定代码生成分析时确定归约的表达式与归约方法
    CfTreeNode(const std::string& key, const std::vector<CfTreeNode*>& cnodes, int reducedExprNumber, const std::string& action, 
        const std::string& reductionAction) 
        : _cnodes(cnodes), _key(key), _reducedExprNumber(reducedExprNumber), _action(action), _reductionAction(reductionAction), _index(-1) {
        for (int i = 0; i < this->_cnodes.size(); i++) {
            this->_cnodes[i]->_pnode = this;
            this->_cnodes[i]->_index = i;
        }
    }

    Attribute* GetAttribute(const std::string& name) {
        auto itr = this->_attributes.find(name);
        if (itr == this->_attributes.end()) {
            return nullptr;
        } else {
            return itr->second;
        }
    }

    bool AddAttribute(Attribute* attribute) {
        if (this->_attributes.find(attribute->_name) != this->_attributes.end()) {
            return false;
        }
        this->_attributes.insert(std::pair<std::string, Attribute*>(attribute->_name, attribute));
        return true;
    }

    bool RemoveAttribute(const std::string& name) {
        auto itr = this->_attributes.find(name);
        if (itr == this->_attributes.end()) {
            return false;
        } else {
            this->_attributes.erase(itr);
            return true;
        }
    }

    static bool MoveAttribute(CfTreeNode* src, CfTreeNode* dest, const std::string& attrName) {
        auto attr = src->GetAttribute(attrName);
        if (attr == nullptr) {
            return false;
        } else {
            return dest->AddAttribute(attr);
        }
    }

    static void MoveAttributes(CfTreeNode* src, CfTreeNode* dest) {
        for (auto itr : src->_attributes) {
            dest->AddAttribute(itr.second);
        }
        src->_attributes.clear();
    }

    static void DestroyTree(CfTreeNode* root) {
        if (root == nullptr) {
            return;
        }
        for (int i = 0; i < root->_cnodes.size(); i++) {
            DestroyTree(root->_cnodes[i]);
        }
        for (auto itr : root->_attributes) {
            delete itr.second;
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

#endif