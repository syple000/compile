#define GEN_AUX_CODE_FILE 1
#ifndef AUX_CODE
#define AUX_CODE 1

#include "../cf_analysis_info.h"
 
#include "../../doc/attribute.h"

static SymbolTableManager tableManager(1000);

class AuxCode {
public:

    std::unordered_map<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)> funcRegistry;
    static void _func_1_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 2], kinfos[kinfos.size() - 1]);
    }
    static void _func_reduction_3_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
 
        CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], kinfos[kinfos.size() - 2]);
        auto typeAttr = (TypeAttribute*)kinfos[kinfos.size() - 2].GetAttribute("type");
        auto ptrAttr = (PtrAttribute*)kinfos[kinfos.size() - 2].GetAttribute("pointer");
        auto arrAttr = (ArrAttribute*)kinfos[kinfos.size() - 2].GetAttribute("array");
        VarType* varType = new VarType(typeAttr->_typeName, typeAttr->_typeSize, 
        ptrAttr == nullptr ? nullptr : ptrAttr->_ptr, arrAttr == nullptr ? nullptr : arrAttr->_array);
        if (ptrAttr != nullptr) {
            ptrAttr->_ptr = nullptr;
        }
        if (arrAttr != nullptr) {
            arrAttr->_array = nullptr;
        }
        tableManager.AddVar(varType, kinfos[kinfos.size() - 2]._value, 
            ((QualifierAttribute*)kinfos[kinfos.size() - 2].GetAttribute("qualifier"))->_isStatic);
    }
    static void _func_reduction_4_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        pinfo.AddAttribute(new QualifierAttribute("qualifier", true));
    }
    static void _func_reduction_5_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        pinfo.AddAttribute(new QualifierAttribute("qualifier", false));
    }
    static void _func_reduction_6_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 2], pinfo);
        auto type = tableManager.GetTable(kinfos[kinfos.size() - 1]._value);
        if (type == nullptr || type->_type != 1) {
            std::cout << "type: " << kinfos[kinfos.size() - 1]._value << "not found!" << std::endl;
        }
        pinfo.AddAttribute(new TypeAttribute("type", kinfos[kinfos.size() - 1]._value, 4));
    }
    static void _func_reduction_7_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 2], pinfo);
        auto type = tableManager.GetTable(kinfos[kinfos.size() - 1]._value);
        if (type == nullptr || type->_type != 1) {
            std::cout << "type: " << kinfos[kinfos.size() - 1]._value << "not found!" << std::endl;
        }
        pinfo.AddAttribute(new TypeAttribute("type", kinfos[kinfos.size() - 1]._value, 4));
    }
    static void _func_2_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 4], kinfos[kinfos.size() - 1]);
        auto arrAttr = (ArrAttribute*)kinfos[kinfos.size() - 1].GetAttribute("array");
        if (arrAttr == nullptr) {
            arrAttr = new ArrAttribute("array");
            kinfos[kinfos.size() - 1].AddAttribute(arrAttr);
        }
        auto typeAttr = (TypeAttribute*)kinfos[kinfos.size() - 1].GetAttribute("type");
        int typeSize = typeAttr->_typeSize;
        if (kinfos[kinfos.size() - 1].GetAttribute("pointer") != nullptr) {
            typeSize = 4;
        }
        arrAttr->AddNestArr(typeSize, std::stoi(kinfos[kinfos.size() - 2]._value));
    }
    static void _func_reduction_8_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
    }
    static void _func_reduction_9_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 2], pinfo);
    }
    static void _func_3_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 2], kinfos[kinfos.size() - 1]);
        auto ptrAttr = (PtrAttribute*)kinfos[kinfos.size() - 2].GetAttribute("pointer");
        if (ptrAttr == nullptr) {
            ptrAttr = new PtrAttribute("pointer");
            kinfos[kinfos.size() - 1].AddAttribute(ptrAttr);
        }
        ptrAttr->AddNestPtr();
    }
    static void _func_reduction_10_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
    }
    static void _func_reduction_11_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 2], pinfo);
    }

    void registFuncs() {
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_1_", _func_1_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_3_", _func_reduction_3_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_4_", _func_reduction_4_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_5_", _func_reduction_5_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_6_", _func_reduction_6_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_7_", _func_reduction_7_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_2_", _func_2_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_8_", _func_reduction_8_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_9_", _func_reduction_9_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_3_", _func_3_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_10_", _func_reduction_10_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_11_", _func_reduction_11_));
    }
};
#endif
