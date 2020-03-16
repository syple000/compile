#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cassert>
#include <set>
#include <unistd.h>
#include <queue>
#include <unordered_map>

#include "./regular_expr/regexpr_engine.h"
#include "./io/io.h"
#include "./context_free/cf_expr.h"
#include "./context_free/cf_engine.h"
#include "./io/format_conversion.h"
#include "./string_util/string_util.h"
#include "./bitset/bitset.h"
#include "./ir/instr.h"

void BitSetTest() {
    BitSet bitSet1(8);
    bitSet1.Set(2);
    bitSet1.Set(3);
    bitSet1.Set(4);
    bitSet1.Clear(3);
    assert(bitSet1.ToString() == "00101000");
    bitSet1.Set(8);
    assert(bitSet1.ToString() == "00101000 10000000 00000000 00000000");
    
    BitSet bitSet2(8), bitSet3(8);
    bitSet2.Set(0);
    bitSet2.Set(2);
    bitSet1.Union(bitSet2);
    assert(bitSet1.ToString() == "10101000 10000000 00000000 00000000");
    assert(bitSet1.Exist(0));
    assert(!bitSet1.Exist(1));
    assert(bitSet1.Exist(8));
    assert(!bitSet1.Exist(16));
    assert(!bitSet1.Exist(64));
    bitSet1.Except(bitSet2);
    assert(bitSet1.ToString() == "00001000 10000000 00000000 00000000");
    bitSet1.Set(0);
    bitSet1.Intersect(bitSet2);
    assert(bitSet1.ToString() == "10000000 00000000 00000000 00000000");
    assert(!bitSet1.Empty());
    bitSet3.Set(0);
    assert(bitSet3 == bitSet1);
    bitSet3.Set(7);
    assert(!(bitSet3 == bitSet1));
    bitSet1.Clear(0);
    assert(bitSet1.Empty());
}

void RegExprTest() {
    std::string repat1 = "(((a)(b|cd*|e)))f*";
    std::string repat2 = "e(a|bb|c)**d\\[\\]";
    std::string repat3 = "a*";
    std::string repat4 = "[0-9][0-9]*";

    std::string matchStr = "((({((a))*})|((a))*)*)";
    for (int i = 0; i < 1; i++) {
        matchStr = "({" + matchStr + "*})|" + matchStr;
    }
    RegExprEngine regExprEngine5(matchStr);
    assert(regExprEngine5.IsMatched("{aa{aaa}{}aa{}{}}"));

    RegExprEngine regExprEngine1(repat1);
    RegExprEngine regExprEngine2(repat2);
    RegExprEngine regExprEngine3(repat3);
    RegExprEngine regExprEngine4(repat4);

    assert(regExprEngine1.InitSuccess());
    assert(regExprEngine2.InitSuccess());
    assert(regExprEngine3.InitSuccess());
    assert(regExprEngine4.InitSuccess());
    
    assert(regExprEngine1.IsMatched("ab"));
    assert(regExprEngine1.IsMatched("ae"));
    assert(regExprEngine1.IsMatched("acf"));
    assert(regExprEngine1.IsMatched("acdddff"));
    assert(regExprEngine1.IsMatched("abfffff"));

    assert(regExprEngine2.IsMatched("ed[]"));
    assert(regExprEngine2.IsMatched("eaaad[]"));
    assert(regExprEngine2.IsMatched("ebbbbd[]"));
    assert(regExprEngine2.IsMatched("ecd[]"));

    assert(regExprEngine3.IsMatched(""));
    assert(regExprEngine3.IsMatched("aa"));

    assert(!regExprEngine3.IsMatched("b"));

    assert(regExprEngine4.IsMatched("123"));
    assert(regExprEngine4.IsMatched("0111"));
}

void StringUtilTest() {
    std::vector<std::string> strs = StringUtil::Split(";;123456;1333114353;1", ";*1");
    assert(strs[0] == "23456");
    assert(strs[1] == "333");
    assert(strs[2] == "4353");
}

void InstructionListTest() {
    InstrList instrList1, instrList2, instrList3;
    assert(instrList1.GetCurInstr() == nullptr);
    instrList1.LocFirst();
    assert(instrList1.GetCurInstr() == nullptr);
    instrList1.LocLast();
    assert(instrList1.GetCurInstr() == nullptr);    
    instrList1.InsertInstr(new Instruction({"1"}));
    instrList1.LocFirst();
    assert(instrList1.GetCurInstr()->_components[0] == "1");
    instrList1.LocLast();
    instrList1.InsertInstr(new Instruction({"2"}));
    instrList1.InsertInstr(new Instruction({"3"}));
    instrList1.InsertInstr(new Instruction({"4"}));
    instrList1.GoBack();
    instrList1.GoBack();
    instrList1.GoBack();
    instrList1.GoAhead();
    assert(instrList1.GetCurInstr()->_components[0] == "3");
    instrList1.LocLast();
    instrList1.GoBack();
    assert(instrList1.GetCurInstr()->_components[0] == "4");
    assert(instrList1.GoAhead() == nullptr);
    instrList1.GoBack();
    instrList1.GoBack();
    delete instrList1.RemoveInstr();
    assert(instrList1.GetCurInstr()->_components[0] == "4");
    assert(instrList1.GetSize() == 3);
    instrList1.GoBack();
    instrList1.GoBack();
    assert(instrList1.GoBack() == nullptr);
    assert(instrList1.GetCurInstr()->_components[0] == "1");
    instrList1.GoAhead();
    instrList1.GoAhead();
    instrList1.InsertInstr(new Instruction({"3"}));
    instrList1.LocLast();

    instrList2.InsertInstr(new Instruction({"5"}));
    instrList2.InsertInstr(new Instruction({"6"}));

    instrList1.MergeInstrList(instrList2);
    assert(instrList1.GetSize() == 6);

    instrList3.InsertInstr(new Instruction({"0"}));
    instrList1.LocFirst();
    instrList1.MergeInstrList(instrList3);
    assert(instrList1.GetSize() == 7);

    instrList1.LocFirst();
    for (int i = 0; i < 7; i++) {
        assert(instrList1.GetCurInstr()->_components[0] == std::to_string(i));
        instrList1.GoAhead();
    }

    for (int i = 6; i >=0; i--) {
        instrList1.GoBack();
        assert(instrList1.GetCurInstr()->_components[0] == std::to_string(i));
    }

    instrList1.LocFirst();
    for (int i = 0; i < 7; i++) {
        delete instrList1.RemoveInstr();
    }

    InstrList instrList4("./debug/resolvable_file/instr.txt");
    instrList4.LocFirst();
    while (instrList4.GetCurInstr() != nullptr) {
        // auto instr = instrList4.GetCurInstr();
        // if (instr->_label.size() != 0) {
        //     std::cout << instr->_label << ": ";
        // }
        // for (auto component : instr->_components) {
        //     std::cout << component << " ";
        // }
        // std::cout << std::endl;
        instrList4.GoAhead();
    }
}

void CfEngineTest() {
    CfEngine cfEngine("./debug/resolvable_file/expr_lexical_file.txt", "./debug/resolvable_file/expr_file.txt", "./debug/resolvable_file/lexical_file.txt");
    assert(cfEngine.InitSuccess());
    cfEngine.GenCfAnalysisInfo("./debug/resolvable_file/code_file.txt");
}

int main() {
    char pwd[256];
    getcwd(pwd, sizeof(pwd));
    std::cout << "pwd: " << pwd << std::endl;
    BitSetTest();
    RegExprTest();
    StringUtilTest();
    InstructionListTest();

    CfEngineTest();
    std::cout << std::endl << "test over!" << std::endl;
    return 0;
}