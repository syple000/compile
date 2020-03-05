#define GEN_AUX_CODE_FILE 1
#ifndef AUX_CODE
#define AUX_CODE 1

#include "../cf_analysis_info.h"
 
#include "../../ir/instr.h"

static InstrFlow instrFlow;

static void ShowInstr(Instruction* instr) {
    if (instr->_label.size() != 0) {
        std::cout << instr->_label << ": ";
    }
    for (auto str : instr->_components) {
        std::cout << str << " ";
    }
    std::cout << std::endl;
}

class AuxCode {
public:

    std::unordered_map<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)> funcRegistry;
    static void _func_reduction_2_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        instrFlow.InsertInstr({"goto", "end"});
        instrFlow.Traverse(ShowInstr);
    
    }
    static void _func_1_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        kinfos[kinfos.size() - 1]._value = "label";
    
    }
    static void _func_2_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        auto boolAttr = (BackFillAttr*)kinfos[kinfos.size() - 2].GetAttribute("back_fill");
        instrFlow.AddNextInstrFillBackInfo("true", boolAttr);
    
    }
    static void _func_3_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        auto label = kinfos[kinfos.size() - 4]._value;
        instrFlow.InsertInstr({"goto", label});
    
    }
    static void _func_reduction_3_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 3], pinfo);
        auto boolAttr = (BackFillAttr*)pinfo.GetAttribute("back_fill");
        auto stmtAttr = (BackFillAttr*)kinfos[kinfos.size() - 1].GetAttribute("back_fill");
        if (stmtAttr != nullptr) {
            boolAttr->AddAllListElems("break", stmtAttr);
        }
        instrFlow.AddNextInstrFillBackInfo("false", boolAttr);
        instrFlow.AddNextInstrFillBackInfo("break", boolAttr);
    
    }
    static void _func_reduction_4_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        instrFlow.InsertInstr({"goto", ""});
        auto attr = new BackFillAttr();
        attr->AddListElem("break", instrFlow.GetLast(), 1);
        pinfo.AddAttribute(attr);
    
    }
    static void _func_reduction_5_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        instrFlow.InsertInstr({kinfos[kinfos.size() - 2]._value});
    
    }
    static void _func_reduction_6_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        instrFlow.InsertInstr({"goto", ""});
        auto attr = new BackFillAttr();
        attr->AddListElem("true", instrFlow.GetLast(), 1);
        pinfo.AddAttribute(attr);
        if (kinfos.size() > 1) {
            if (kinfos[kinfos.size() - 2]._value == "label" && instrFlow.GetLast()->_label.size() == 0) {
                instrFlow.AddLabel(instrFlow.GetLast());
            }
            kinfos[kinfos.size() - 2]._value = instrFlow.GetLast()->_label;
        }
    
    }
    static void _func_reduction_7_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        instrFlow.InsertInstr({"goto", ""});
        auto attr = new BackFillAttr();
        attr->AddListElem("false", instrFlow.GetLast(), 1);
        pinfo.AddAttribute(attr); 
        if (kinfos.size() > 1) {
            if (kinfos[kinfos.size() - 2]._value == "label" && instrFlow.GetLast()->_label.size() == 0) {
                instrFlow.AddLabel(instrFlow.GetLast());
            }
            kinfos[kinfos.size() - 2]._value = instrFlow.GetLast()->_label;
        }
    
    }
    static void _func_5_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        auto boolAttr = (BackFillAttr*)kinfos[kinfos.size() - 2].GetAttribute("back_fill");
        instrFlow.AddNextInstrFillBackInfo("false", boolAttr);
    
    }
    static void _func_reduction_8_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
        auto boolAttr = (BackFillAttr*)kinfos[kinfos.size() - 3].GetAttribute("back_fill");
        auto pattr = (BackFillAttr*)pinfo.GetAttribute("back_fill");
        pattr->AddAllListElems("true", boolAttr);
    
    }
    static void _func_reduction_9_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
    
    }
    static void _func_6_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        auto joinAttr = (BackFillAttr*)kinfos[kinfos.size() - 2].GetAttribute("back_fill");
        instrFlow.AddNextInstrFillBackInfo("true", joinAttr);
    
    }
    static void _func_reduction_10_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
        auto pattr = (BackFillAttr*)pinfo.GetAttribute("back_fill");
        auto joinAttr = (BackFillAttr*)kinfos[kinfos.size() - 3].GetAttribute("back_fill");
        pattr->AddAllListElems("false", joinAttr);
    
    }
    static void _func_reduction_11_(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {

        CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
    
    }

    void registFuncs() {
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_2_", _func_reduction_2_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_1_", _func_1_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_2_", _func_2_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_3_", _func_3_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_3_", _func_reduction_3_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_4_", _func_reduction_4_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_5_", _func_reduction_5_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_6_", _func_reduction_6_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_7_", _func_reduction_7_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_5_", _func_5_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_8_", _func_reduction_8_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_9_", _func_reduction_9_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_6_", _func_6_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_10_", _func_reduction_10_));
        funcRegistry.insert(std::pair<std::string, void(*)(CfInfo&, std::vector<CfInfo>&)>("_func_reduction_11_", _func_reduction_11_));
    }
};
#endif
