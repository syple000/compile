#include <stack>
#include <set>
#include <vector>
#include <string>

#ifndef REGULAR_EXPR_ANALYSIS_TREE
#define REGULAR_EXPR_ANALYSIS_TREE 1

#define ESCAPE '\\'

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

class RegExprAnalysisTree {
private:

    inline unsigned char GetCharactorOfIndex(const std::string& expr, int index);

    inline bool IsConnectable(unsigned char ch, int priority);

    inline void SetNext(const std::set<RegExprNode*>* preSet, const std::set<RegExprNode*>* postSet);

    inline void DestroyElems(std::stack<RegExprNode*>&);

    bool DoCalc(std::stack<unsigned char>& ops, std::stack<RegExprNode*>& elems, 
        unsigned char nextOp, int priority);

    int GetPriority(unsigned char);

public:
    RegExprAnalysisTree() = default;
    virtual ~RegExprAnalysisTree() = default;
    RegExprNode* Analyze(const std::string&);
};

#endif