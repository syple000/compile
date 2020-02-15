#define GEN_AUX_CODE_FILE 1
#ifndef AUX_CODE
#define AUX_CODE 1

#include "../../tree_node/tree_node.h"
 
#include "../../doc/attribute.h"

static SymbolTableManager tableManager(1000);

class AuxCode {
public:

    std::unordered_map<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)> funcRegistry;
    static void _func_1_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 2], knodes[knodes.size() - 1]);
    }
    static void _func_reduction_3_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {
 
        CfTreeNode::MoveAttributes(knodes[knodes.size() - 1], knodes[knodes.size() - 2]);
        auto typeAttr = (TypeAttribute*)knodes[knodes.size() - 2]->GetAttribute("type");
        auto ptrAttr = (PtrAttribute*)knodes[knodes.size() - 2]->GetAttribute("pointer");
        auto arrAttr = (ArrAttribute*)knodes[knodes.size() - 2]->GetAttribute("array");
        VarType* varType = new VarType(typeAttr->_typeName, typeAttr->_typeSize, 
        ptrAttr == nullptr ? nullptr : ptrAttr->_ptr, arrAttr == nullptr ? nullptr : arrAttr->_array);
        if (ptrAttr != nullptr) {
            ptrAttr->_ptr = nullptr;
        }
        if (arrAttr != nullptr) {
            arrAttr->_array = nullptr;
        }
        tableManager.AddVar(varType, knodes[knodes.size() - 2]->_value, 
            ((QualifierAttribute*)knodes[knodes.size() - 2]->GetAttribute("qualifier"))->_isStatic);
    }
    static void _func_reduction_4_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        pnode->AddAttribute(new QualifierAttribute("qualifier", true));
    }
    static void _func_reduction_5_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        pnode->AddAttribute(new QualifierAttribute("qualifier", false));
    }
    static void _func_reduction_6_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 2], pnode);
        auto type = tableManager.GetTable(knodes[knodes.size() - 1]->_value);
        if (type == nullptr || type->_type != 1) {
            std::cout << "type: " << knodes[knodes.size() - 1]->_value << "not found!" << std::endl;
        }
        pnode->AddAttribute(new TypeAttribute("type", knodes[knodes.size() - 1]->_value, 4));
    }
    static void _func_reduction_7_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 2], pnode);
        auto type = tableManager.GetTable(knodes[knodes.size() - 1]->_value);
        if (type == nullptr || type->_type != 1) {
            std::cout << "type: " << knodes[knodes.size() - 1]->_value << "not found!" << std::endl;
        }
        pnode->AddAttribute(new TypeAttribute("type", knodes[knodes.size() - 1]->_value, 4));
    }
    static void _func_2_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 4], knodes[knodes.size() - 1]);
        auto arrAttr = (ArrAttribute*)knodes[knodes.size() - 1]->GetAttribute("array");
        if (arrAttr == nullptr) {
            arrAttr = new ArrAttribute("array");
            knodes[knodes.size() - 1]->AddAttribute(arrAttr);
        }
        auto typeAttr = (TypeAttribute*)knodes[knodes.size() - 1]->GetAttribute("type");
        int typeSize = typeAttr->_typeSize;
        if (knodes[knodes.size() - 1]->GetAttribute("pointer") != nullptr) {
            typeSize = 4;
        }
        arrAttr->AddNestArr(typeSize, std::stoi(knodes[knodes.size() - 2]->_value));
    }
    static void _func_reduction_8_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 1], pnode);
    }
    static void _func_reduction_9_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 2], pnode);
    }
    static void _func_3_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 2], knodes[knodes.size() -1]);
        auto ptrAttr = (PtrAttribute*)knodes[knodes.size() - 2]->GetAttribute("pointer");
        if (ptrAttr == nullptr) {
            ptrAttr = new PtrAttribute("pointer");
            knodes[knodes.size() - 1]->AddAttribute(ptrAttr);
        }
        ptrAttr->AddNestPtr();
    }
    static void _func_reduction_10_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 1], pnode);
    }
    static void _func_reduction_11_(CfTreeNode* pnode, std::vector<CfTreeNode*>& knodes) {

        CfTreeNode::MoveAttributes(knodes[knodes.size() - 2], pnode);
    }

    void registFuncs() {
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_1_", _func_1_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_3_", _func_reduction_3_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_4_", _func_reduction_4_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_5_", _func_reduction_5_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_6_", _func_reduction_6_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_7_", _func_reduction_7_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_2_", _func_2_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_8_", _func_reduction_8_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_9_", _func_reduction_9_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_3_", _func_3_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_10_", _func_reduction_10_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>&)>("_func_reduction_11_", _func_reduction_11_));
    }
};
#endif
