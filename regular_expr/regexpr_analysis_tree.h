#include <stack>
#include <set>
#include <vector>
#include <string>

#include "../tree_node/tree_node.h"

#ifndef REGULAR_EXPR_ANALYSIS_TREE
#define REGULAR_EXPR_ANALYSIS_TREE 1

#define ESCAPE '\\'

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