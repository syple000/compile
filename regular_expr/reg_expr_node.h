#include <vector>
#include <string>
#include <set>
#include <iostream>

#ifndef DEBUG_CODE
#define DEBUG_CODE 1
#endif

#ifndef TREE_NODE
#define TREE_NODE 1

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