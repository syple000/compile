#include "../ir/instr.h"

static InstrFlow instrFlow;

// 1 STMTS STMT STMTS;

// 2 STMTS null {1};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    instrFlow.InsertInstr({"goto", "end"});
}

// 3 STMT while lp {1} BOOL rp {2} STMT {3} {4};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    kinfos[kinfos.size() - 1]._value = instrFlow.GetLabel();
}

static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    auto boolAttr = (BackFillAttr*)kinfos[kinfos.size() - 2].GetAttribute("back_fill");
    instrFlow.AddNextInstrFillBackInfo("true", boolAttr);
}

static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    instrFlow.InsertInstr({"goto", kinfos[kinfos.size() - 4]._value});
}

static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    auto boolAttr = (BackFillAttr*)kinfos[kinfos.size() - 3].GetAttribute("back_fill");
    instrFlow.AddNextInstrFillBackInfo("false", boolAttr);
    auto breakAttr = (BackFillAttr*)kinfos[kinfos.size() - 1].GetAttribute("back_fill");
    instrFlow.AddNextInstrFillBackInfo("break", breakAttr);
}

// 4 STMT break {1};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    instrFlow.InsertInstr({"goto", ""});
    auto attr = new BackFillAttr();
    attr->AddListElem("break", instrFlow.GetTail(), 1);
    pinfo.AddAttribute(attr);
}

// 5 STMT id sem {1};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    instrFlow.InsertInstr({kinfos[kinfos.size() - 1]._value});
}

// 6 TAF true {1};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    instrFlow.InsertInstr({"goto", ""});
    auto attr = new BackFillAttr();
    attr->AddListElem("true", instrFlow.GetTail(), 1);
    pinfo.AddAttribute(attr);
    if (kinfos.size() > 1) {
        if (kinfos[kinfos.size() - 2]._value == "label") {
            instrFlow.GetTail()->_label = instrFlow.GetLabel();
            kinfos[kinfos.size() - 2]._value = instrFlow.GetTail()->_label;
        }
    }
}

// 7 TAF false{1};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    instrFlow.InsertInstr({"goto", ""});
    auto attr = new BackFillAttr();
    attr->AddListElem("false", instrFlow.GetTail(), 1);
    pinfo.AddAttribute(attr);
    if (kinfos.size() > 1) {
        if (kinfos[kinfos.size() - 2]._value == "label") {
            instrFlow.GetTail()->_label = instrFlow.GetLabel();
            kinfos[kinfos.size() - 2]._value = instrFlow.GetTail()->_label;
        }
    }
}

// 8 BOOL BOOL or {1} JOIN {2};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    auto boolAttr = (BackFillAttr*)kinfos[kinfos.size() - 2].GetAttribute("back_fill");
    instrFlow.AddNextInstrFillBackInfo("false", boolAttr);
}

static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
    auto boolAttr = (BackFillAttr*)kinfos[kinfos.size() - 3].GetAttribute("back_fill");
    auto pattr = (BackFillAttr*)pinfo.GetAttribute("back_fill");
    pattr->AddAllListElems("true", boolAttr);
}

// 9 BOOL JOIN {1};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
}

// 10 JOIN JOIN and {1} TAF {2};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    auto joinAttr = (BackFillAttr*)kinfos[kinfos.size() - 2].GetAttribute("back_fill");
    instrFlow.AddNextInstrFillBackInfo("true", joinAttr);
}

static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
    auto pattr = (BackFillAttr*)pinfo.GetAttribute("back_fill");
    auto joinAttr = (BackFillAttr*)kinfos[kinfos.size() - 3].GetAttribute("back_fill");
    pattr->AddAllListElems("false", joinAttr);
}

// 11 JOIN TAF {1};
static void func(CfInfo& pinfo, std::vector<CfInfo>& kinfos) {
    CfInfo::MoveAttributes(kinfos[kinfos.size() - 1], pinfo);
}