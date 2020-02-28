#include "./attribute.h"

// 该文件不做编译使用
static SymbolTableManager tableManager; 

// DECL QLFR TYPE PTR id "1" ARR "2"
static void _func_1_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 2], knodes[knodes.size() - 1]);
}

static void _func_2_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 1], knodes[knodes.size() - 2]);
    auto typeAttr = (TypeAttribute*)knodes[knodes.size() - 2].GetAttribute("type");
    auto ptrAttr = (PtrAttribute*)knodes[knodes.size() - 2].GetAttribute("pointer");
    auto arrAttr = (ArrAttribute*)knodes[knodes.size() - 2].GetAttribute("array");
    VarType* varType = new VarType(typeAttr->_typeName, typeAttr->_typeSize, 
    ptrAttr == nullptr ? nullptr : ptrAttr->_ptr, arrAttr == nullptr ? nullptr : arrAttr->_array);
    if (ptrAttr != nullptr) {
        ptrAttr->_ptr = nullptr;
    }
    if (arrAttr != nullptr) {
        arrAttr->_array = nullptr;
    }
    tableManager.AddVar(varType, knodes[knodes.size() - 2]._value, 
        ((QualifierAttribute*)knodes[knodes.size() - 2].GetAttribute("qualifier"))->_isStatic);
}

// QLFR static
static void _func_1_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    pnode.AddAttribute(new QualifierAttribute("qualifier", true));
}

// QLFR null
static void _func_1_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    pnode.AddAttribute(new QualifierAttribute("qualifier", false));
}

// TYPE int
// TYPE id
static void _func_1_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 2], pnode);
    auto type = tableManager.GetTable(knodes[knodes.size() - 1]._value);
    if (type == nullptr || type->_type != 1) {
        std::cout << "type: " << knodes[knodes.size() - 1]._value << "not found!" << std::endl;
    }
    pnode.AddAttribute(new TypeAttribute("type", knodes[knodes.size() - 1]._value, 4));
}

// ARR ls num rs "1" ARR "2"
static void _func_1_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 4], knodes[knodes.size() - 1]);
    auto arrAttr = (ArrAttribute*)knodes[knodes.size() - 1].GetAttribute("array");
    if (arrAttr == nullptr) {
        arrAttr = new ArrAttribute("array");
        knodes[knodes.size() - 1].AddAttribute(arrAttr);
    }
    auto typeAttr = (TypeAttribute*)knodes[knodes.size() - 1].GetAttribute("type");
    int typeSize = typeAttr->_typeSize;
    if (knodes[knodes.size() - 1].GetAttribute("pointer") != nullptr) {
        typeSize = 4;
    }
    arrAttr->AddNestArr(typeSize, std::stoi(knodes[knodes.size() - 2]._value));
}

static void _func_2_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 1], pnode);
}

// ARR null
static void _func_1_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 2], pnode);
}

// PTR ast "1" PTR "2"
static void _func_1_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 2], knodes[knodes.size() -1]);
    auto ptrAttr = (PtrAttribute*)knodes[knodes.size() - 2].GetAttribute("pointer");
    if (ptrAttr == nullptr) {
        ptrAttr = new PtrAttribute("pointer");
        knodes[knodes.size() - 1].AddAttribute(ptrAttr);
    }
    ptrAttr->AddNestPtr();
}

static void _func_2_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 1], pnode);
}

// PTR null
static void _func_1_(CfInfo& pnode, std::vector<CfInfo>& knodes) {
    CfInfo::MoveAttributes(knodes[knodes.size() - 2], pnode);
}