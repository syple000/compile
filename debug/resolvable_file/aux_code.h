#define GEN_AUX_CODE_FILE 1
#ifndef AUX_CODE
#define AUX_CODE 1

#include "../../tree_node/tree_node.h"
 
static int res;

class AuxCode {
public:

    std::unordered_map<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)> funcRegistry;
    static void _func_0_(CfTreeNode* pnode, std::vector<CfTreeNode*> cnodes) {

    res = std::stoi(cnodes[cnodes.size() - 3]->_value) + std::stoi(cnodes[cnodes.size() - 1]->_value);
    pnode->_value = std::to_string(res);
    }
    static void _func_1_(CfTreeNode* pnode, std::vector<CfTreeNode*> cnodes) {

    res = std::stoi(cnodes[cnodes.size() - 3]->_value) - std::stoi(cnodes[cnodes.size() - 1]->_value);
    pnode->_value = std::to_string(res);
    }
    static void _func_2_(CfTreeNode* pnode, std::vector<CfTreeNode*> cnodes) {

    pnode->_value = cnodes[cnodes.size() - 1]->_value;
    }
    static void _func_5_(CfTreeNode* pnode, std::vector<CfTreeNode*> cnodes) {

    pnode->_value = cnodes[cnodes.size() - 1]->_value;
    }

    void registFuncs() {
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_0_", _func_0_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_1_", _func_1_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_2_", _func_2_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_5_", _func_5_));
    }
};
#endif
