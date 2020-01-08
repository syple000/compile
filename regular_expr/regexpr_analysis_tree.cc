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

    default:
        return -1;
    }
}

RegExprNode* RegExprAnalysisTree::Analyze(const std::string& expr) {
    std::stack<unsigned char> ops;
    std::stack<RegExprNode*> elems;
    bool connectable = false;
    int indexOfExpr = 0;
    std::string analyzedExpr = "(" + expr + ")";
    while (indexOfExpr <= analyzedExpr.size()) {
        char ch = RegExprAnalysisTree::GetCharactorOfIndex(analyzedExpr, indexOfExpr++);
        int priority;
        if (ch == ESCAPE) {
            ch = RegExprAnalysisTree::GetCharactorOfIndex(analyzedExpr, indexOfExpr++);
            priority = -1;
        } else {
            priority = RegExprAnalysisTree::GetPriority(ch);
        }

        if (connectable && (priority == -1 || ch == '(')) {
            RegExprAnalysisTree::DoCalc(ops, elems, '.', RegExprAnalysisTree::GetPriority('.'));
            ops.push('.');
        }

        if (priority == -1) {
            elems.push(new RegExprNode(ch, nullptr, nullptr, false));
        } else {
            RegExprAnalysisTree::DoCalc(ops, elems, ch, priority);
            if (ch != ')') {
                ops.push(ch);
            }
        }
        connectable = RegExprAnalysisTree::IsConnectable(ch, priority, elems.empty());
    }

    if (ops.size() == 1 && ops.top() == '.' && elems.size() == 1) {
        elems.push(new RegExprNode('\0', nullptr, nullptr, false));
        RegExprAnalysisTree::DoCalc(ops, elems, '.', RegExprAnalysisTree::GetPriority('.'));
        return elems.top();
    } else {
        return nullptr;
    }
}

void RegExprAnalysisTree::DoCalc(std::stack<unsigned char>& ops, std::stack<RegExprNode*>& elems, 
    unsigned char nextOp, int priority) {
    while (!ops.empty()) {
        char op = ops.top();
        if (RegExprAnalysisTree::GetPriority(op) >= priority) {
            switch (op) {
                case '(': {
                    if (nextOp == ')') {
                        ops.pop();
                    }
                    return;
                }
                
                case '.': {
                    ops.pop();
                    RegExprNode* right = elems.top();
                    elems.pop();
                    RegExprNode* left = elems.top();
                    elems.pop();
                    RegExprNode* newElem = new RegExprNode(op, left, right, left->_nullable && right->_nullable);
                    newElem->_first->insert(left->_first->begin(), left->_first->end());
                    if (left->_nullable) {
                        newElem->_first->insert(right->_first->begin(), right->_first->end());
                    }
                    newElem->_last->insert(right->_last->begin(), right->_last->end());
                    if (right->_nullable) {
                        newElem->_last->insert(left->_last->begin(), left->_last->end());
                    }
                    RegExprAnalysisTree::SetNext(left->_last, right->_first);
                    elems.push(newElem);
                    break;
                }

                case '|': {
                    ops.pop();
                    RegExprNode* right = elems.top();
                    elems.pop();
                    RegExprNode* left = elems.top();
                    elems.pop();
                    RegExprNode* newElem = new RegExprNode(op, left, right, left->_nullable || right->_nullable);
                    newElem->_first->insert(left->_first->begin(), left->_first->end());
                    newElem->_first->insert(right->_first->begin(), right->_first->end());
                    newElem->_last->insert(right->_last->begin(), right->_last->end());
                    newElem->_last->insert(left->_last->begin(), left->_last->end());
                    elems.push(newElem);
                    break;
                }

                case '*': {
                    ops.pop();
                    RegExprNode* left = elems.top();
                    elems.pop();
                    RegExprNode* newElem = new RegExprNode(op, left, nullptr, true);
                    newElem->_first->insert(left->_first->begin(), left->_first->end());
                    newElem->_last->insert(left->_last->begin(), left->_last->end());
                    RegExprAnalysisTree::SetNext(left->_last, left->_first);
                    elems.push(newElem);
                    break;
                }

                default:
                    return;
                    
            }
        } else {
            break;
        }
    }
    return;
}