#include <vector>
#include <string>
#include <set>

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

    CfTreeNode(const std::string& key, const std::vector<CfTreeNode*>& cnodes) : _cnodes(cnodes), _key(key) {
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