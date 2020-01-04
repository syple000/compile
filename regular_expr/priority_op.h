#include <stack>
#include <set>

#ifndef REGULAR_EXPR_PRIOR_OP
#define REGULAR_EXPR_PRIOR_OP

namespace r_e {

    struct RegExprNode {
        unsigned char _content;
        int pos;
        RegExprNode* _leftChildNode, * _rightChildNode;
        std::set<int> _first, _last, _next;
    };

    class PriorityOperation {
    private:
        std::stack<unsigned char> _ops;
        std::stack<unsigned char> _elems;
        int GetPriority(unsigned char);
    public:
        PriorityOperation() = default;
        virtual ~PriorityOperation() = default;
        

    };
}

#endif