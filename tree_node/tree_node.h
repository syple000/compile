#include <vector>
#include <string>

struct CfTreeNode {
    CfTreeNode* _pnode = nullptr;
    std::vector<CfTreeNode*> _cnodes;
    std::string _key;
    std::string _value;
    // generated code
    std::string _code;
};