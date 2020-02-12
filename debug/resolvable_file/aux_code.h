#define GEN_AUX_CODE_FILE 1
#ifndef AUX_CODE
#define AUX_CODE 1

#include "../../tree_node/tree_node.h"
 
static int res;

class AuxCode {
public:

    std::unordered_map<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)> funcRegistry;
    static void _func_0_(CfTreeNode* pnode, std::vector<CfTreeNode*> knodes) {

    std::cout << knodes[knodes.size() - 2]->_value << knodes[knodes.size() - 1]->_value << std::endl;
    res = std::stoi(knodes[knodes.size() - 3]->_value) + std::stoi(knodes[knodes.size() - 1]->_value);
    pnode->_value = std::to_string(res);
    }
    static void _func_1_(CfTreeNode* pnode, std::vector<CfTreeNode*> knodes) {

    std::cout << knodes[knodes.size() - 2]->_value << knodes[knodes.size() - 1]->_value << std::endl;
    res = std::stoi(knodes[knodes.size() - 3]->_value) - std::stoi(knodes[knodes.size() - 1]->_value);
    pnode->_value = std::to_string(res);
    }
    static void _func_2_(CfTreeNode* pnode, std::vector<CfTreeNode*> knodes) {

    std::cout << knodes[knodes.size() - 1]->_value << std::endl;
    pnode->_value = knodes[knodes.size() - 1]->_value;
    }
    static void _func_3_(CfTreeNode* pnode, std::vector<CfTreeNode*> knodes) {

    pnode->_value = knodes[knodes.size() - 2]->_value + " " + knodes[knodes.size() - 1]->_value + " ";
    }
    static void _func_4_(CfTreeNode* pnode, std::vector<CfTreeNode*> knodes) {

    pnode->_value = knodes[knodes.size() - 2]->_value + " " + knodes[knodes.size() - 1]->_value + " ";
    }
    static void _func_5_(CfTreeNode* pnode, std::vector<CfTreeNode*> knodes) {

    pnode->_value = knodes[knodes.size() - 1]->_value;
    }

    void registFuncs() {
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_0_", _func_0_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_1_", _func_1_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_2_", _func_2_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_3_", _func_3_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_4_", _func_4_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)>("_func_5_", _func_5_));
    }
};
#endif
