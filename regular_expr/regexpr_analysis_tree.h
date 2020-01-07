#include <stack>
#include <set>
#include <vector>
#include <string>

#ifndef REGULAR_EXPR_ANALYSIS_TREE
#define REGULAR_EXPR_ANALYSIS_TREE 1

#define ESCAPE '\\'

struct RegExprNode {
    unsigned char _content;
    RegExprNode* _leftChildNode, * _rightChildNode;
    std::set<RegExprNode*> _first, _last, _next;

    RegExprNode(char content, RegExprNode* leftChildNode, RegExprNode* rightChildNode) 
    : _content(content), _leftChildNode(leftChildNode), _rightChildNode(rightChildNode){
        // init first and last
        if (_leftChildNode == nullptr && _rightChildNode == nullptr) {
            _first.insert(this);
            _last.insert(this);
        }
    }

    RegExprNode() = default;
    virtual ~RegExprNode() = default;

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
    int GetPriority(unsigned char);

    unsigned char GetCharactorOfIndex(const std::string& expr, int index) {
        if (index >= expr.size()) {
            return '\0';
        } else {
            return expr[index];
        }
    }

    bool IsConnectable(unsigned char ch, int priority, bool isElemsEmpty) {
        if (priority == -1) {
            return true;
        } else {
            return ch == '*' || (ch == ')' && !isElemsEmpty);
        }
    }

    void SetNext(const std::set<RegExprNode*>& preSet, const std::set<RegExprNode*>& postSet) {
        for (RegExprNode* preNode : preSet) {
            preNode->_next.insert(postSet.begin(), postSet.end());
        }
    }

    void DoCalc(std::stack<unsigned char>& ops, std::stack<RegExprNode*>& elems, 
        unsigned char nextOp, int priority);

public:
    RegExprAnalysisTree() = default;
    virtual ~RegExprAnalysisTree() = default;
    RegExprNode* Analyze(const std::string&);
};

#endif