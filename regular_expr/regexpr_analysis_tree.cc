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

inline void RegExprAnalysisTree::SetNext(const std::set<RegExprNode*>* preSet, const std::set<RegExprNode*>* postSet) {
    for (const auto preNode : *preSet) {
        preNode->_next->insert(postSet->begin(), postSet->end());
    }
}

inline bool RegExprAnalysisTree::IsConnectable(unsigned char ch, int priority) {
    if (priority == -1) {
        return true;
    } else {
        return ch == '*' || (ch == ')');
    }
}

inline unsigned char RegExprAnalysisTree::GetCharactorOfIndex(const std::string& expr, int index) {
    if (index >= expr.size()) {
        return '.';
    } else {
        return expr[index];
    }
}

inline void RegExprAnalysisTree::DestroyElems(std::stack<RegExprNode*>& elems) {
    while (!elems.empty()) {
        RegExprNode* root = elems.top();
        elems.pop();
        RegExprNode::DestroyTree(root);
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
            if (!RegExprAnalysisTree::DoCalc(ops, elems, '.', RegExprAnalysisTree::GetPriority('.'))) {
                this->DestroyElems(elems);
                return nullptr;
            }
            ops.push('.');
        }

        if (priority == -1) {
            elems.push(new RegExprNode(ch, nullptr, nullptr, false));
        } else {
            if (!RegExprAnalysisTree::DoCalc(ops, elems, ch, priority)) {
                this->DestroyElems(elems);
                return nullptr;
            }
            if (ch != ')') {
                ops.push(ch);
            }
        }
        connectable = RegExprAnalysisTree::IsConnectable(ch, priority);
    }

    if (ops.size() == 1 && ops.top() == '.' && elems.size() == 1) {
        elems.push(new RegExprNode('\0', nullptr, nullptr, false));
        RegExprAnalysisTree::DoCalc(ops, elems, '.', RegExprAnalysisTree::GetPriority('.'));
        return elems.top();
    } else {
        this->DestroyElems(elems);
        return nullptr;
    }
}

bool RegExprAnalysisTree::DoCalc(std::stack<unsigned char>& ops, std::stack<RegExprNode*>& elems, 
    unsigned char nextOp, int priority) {
    while (!ops.empty()) {
        char op = ops.top();
        if (RegExprAnalysisTree::GetPriority(op) >= priority) {
            switch (op) {
                case '(': {
                    if (nextOp == ')') {
                        ops.pop();
                    }
                    return true;
                }
                
                case '.': {
                    if (elems.size() < 2) {
                        return false;
                    }
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
                    if (elems.size() < 2) {
                        return false;
                    }
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
                    if (elems.size() < 1) {
                        return false;
                    }
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
                    return false;
                    
            }
        } else {
            break;
        }
    }
    return true;
}