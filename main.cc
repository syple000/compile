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

int main() {
    char pwd[256];
    getcwd(pwd, sizeof(pwd));
    std::cout << "pwd: " << pwd << std::endl;
    // bitset test
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
    bitSet3.Set(0);
    assert(bitSet3 == bitSet1);
    bitSet3.Set(7);
    assert(!(bitSet3 == bitSet1));

    // regular expr test
    // std::string repat = "((a)(b|cd*|e))f*";
    std::string repat1 = "((a)(b|cd*|e))f*";
    std::string repat2 = "e(a|bb|c)**d\\[\\]";
    std::string repat3 = "a*";
    std::string repat4 = "[0-9][0-9]*";
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

    std::vector<std::string> strs = StringUtil::Split(";;123456;1333114353;1", ";*1");
    for (auto str : strs) {
        std::cout << str << " ";
    }
    std::cout << std::endl;
    
    std::cout << StringUtil::Replace(";;123456;1333114353;1", ";*1", "aaa") << std::endl;

    // context free test
    // 该过程会写入aux_code文件; 后续过程需要的正常进行需要再次编译
    CfEngine cfEngine("./debug/resolvable_file/expr_lexical_file.txt", "./debug/resolvable_file/expr_file.txt", "./debug/resolvable_file/lexical_file.txt");
    assert(cfEngine.InitSuccess());

    cfEngine.GenCfAnalysisInfo("./debug/resolvable_file/code_file.txt");

    std::cout << std::endl;

    std::cout << "test over!" << std::endl;
    return 0;
}