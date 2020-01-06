#include "./regexpr_analysis_tree.h"

int RegExprAnalysisTree::GetPriority(unsigned char ch) {
    switch (ch)
    {
    case ')':
        return 1;
    
    case '|':
        return 2;
    
    case '.':
        return 3;

    case '*':
        return 4;

    case '(':
        return 5;

    case '\0':
        return 0;

    default:
        return -1;
    }
}

RegExprNode* RegExprAnalysisTree::Analyze(const std::string& expr) {
    std::stack<unsigned char> ops;
    std::stack<RegExprNode*> elems;
    bool connectable = false;
    int indexOfExpr = 0;
    int nodeNumber = 0;

    while (indexOfExpr <= expr.size()) {
        char ch = RegExprAnalysisTree::GetCharactorOfIndex(expr, indexOfExpr++);
        int priority;
        if (ch == ESCAPE) {
            if (indexOfExpr < expr.size()) {
                ch = RegExprAnalysisTree::GetCharactorOfIndex(expr, indexOfExpr++);
                priority = -1;
            } else {
                return nullptr;
            }
        } else {
            priority = RegExprAnalysisTree::GetPriority(ch);
        }

        if (connectable) {
            if (priority == -1 || ch == '(') {
                nodeNumber = RegExprAnalysisTree::DoCalc(ops, elems, '.', RegExprAnalysisTree::GetPriority('.'),
                    nodeNumber);
                ops.push('.');
                if (priority == -1) {
                    elems.push(new RegExprNode(ch, nodeNumber++, nullptr, nullptr));
                } else {
                    ops.push(ch);
                }
            } else {
                nodeNumber = RegExprAnalysisTree::DoCalc(ops, elems, ch, priority, nodeNumber);
                if (ch != ')') {
                    ops.push(ch);
                }
            }
        } else {
            if (priority == -1) {
                elems.push(new RegExprNode(ch, nodeNumber++, nullptr, nullptr));
            } else {
                if (ch != '(') {
                    return nullptr;
                }
                ops.push(ch);
            }
        }

        connectable = RegExprAnalysisTree::IsConnectable(ch, priority, elems.empty());
    }
    if (ops.size() == 1 && ops.top() == '\0' && elems.size() == 1) {
        return elems.top();
    }
}

int RegExprAnalysisTree::DoCalc(std::stack<unsigned char>& ops, std::stack<RegExprNode*>& elems, 
    unsigned char nextOp, int priority, int nodeNumber) {
    while (!ops.empty()) {
        char op = ops.top();
        if (RegExprAnalysisTree::GetPriority(op) >= priority) {
            switch (op) {
                case '(': 
                    if (nextOp == ')') {
                        ops.pop();
                    }
                    return nodeNumber;
                
                case '.':
                    ops.pop();
                    RegExprNode* right = elems.top();
                    elems.pop();
                    RegExprNode* left = elems.top();
                    elems.pop();
                    RegExprNode* newElem = new RegExprNode(op, nodeNumber++, left, right);
                    
            }
            else if (op == '.') {}
            else if
        }
    }
}