#include <stack>
#include <set>

#ifndef REGULAR_EXPR_ANALYSIS_TREE
#define REGULAR_EXPR_ANALYSIS_TREE 1

#define ESCAPE '\\'

struct RegExprNode {
    unsigned char _content;
    int _pos;
    RegExprNode* _leftChildNode, * _rightChildNode;
    std::set<int> _first, _last, _next;

    RegExprNode(char content, int pos, RegExprNode* leftChildNode, RegExprNode* rightChildNode) 
    : _content(content), _pos(pos), _leftChildNode(leftChildNode), _rightChildNode(rightChildNode){
        // init first and last
        if (_leftChildNode == nullptr && _rightChildNode == nullptr) {
            _first.insert(pos);
            _last.insert(pos);
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

    int DoCalc(std::stack<unsigned char>& ops, std::stack<RegExprNode*>& elems, 
        unsigned char nextOp, int priority, int nodeNumber);

public:
    RegExprAnalysisTree() = default;
    virtual ~RegExprAnalysisTree() = default;
    RegExprNode* Analyze(const std::string&);
};

#endif