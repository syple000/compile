#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cassert>
#include <set>

#include "./regular_expr/regexpr_engine.h"
#include "./io/io.h"
#include "./context_free/cf_expr.h"
#include "./context_free/cf_engine.h"
#include "./io/format_conversion.h"

int main() {
    // matrix io test
    std::vector<std::vector<int>> matrix(10, std::vector<int>(8));
    std::vector<int> vec(10);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            matrix[i][j] = rand() % 100;
        }
        vec[i] = rand() % 100;
    }
    IO<int> io(String2Int, Int2String);
    Buffer buf(100);
    io.AppendVecToBuf(buf, vec);
    io.AppendMatrixToBuf(buf, matrix);
    io.AppendVecToBuf(buf, vec);
    assert(0 == io.WriteFile(buf, "/home/syple/code/test.txt", std::ios::out));
    io.ReadFile(buf, "/home/syple/code/test.txt");
    std::vector<int> rvec = io.GetVecFromBuf(buf);
    std::vector<std::vector<int>> rmatrix = io.GetMatrixFromBuf(buf);
    assert(rvec.size() == 10);
    assert(rmatrix.size() == 10);
    assert(rmatrix[0].size() == 8);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            assert(matrix[i][j] == rmatrix[i][j]);
        }
        assert(rvec[i] == vec[i]);
    }
    assert(buf.GetNextStringSplitByBlank() == "10");
    std::cout << buf.GetNextLine() << std::endl;
    std::cout << buf.GetNextLine() << std::endl;

    // regular expr test
    // std::string repat = "((a)(b|cd*|e))f*";
    std::string repat1 = "((a)(b|cd*|e))f*";
    std::string repat2 = "e(a|bb|c)**d";
    std::string repat3 = "a*";
    RegExprEngine regExprEngine1(repat1, true);
    RegExprEngine regExprEngine2(repat2, true);
    RegExprEngine regExprEngine3(repat3, true);
    if (regExprEngine1.InitSuccess()) {
        std::cout << regExprEngine1.IsMatched("ab") << std::endl;
        std::cout << regExprEngine1.IsMatched("ae") << std::endl;
        std::cout << regExprEngine1.IsMatched("acf") << std::endl;
        std::cout << regExprEngine1.IsMatched("acdddff") << std::endl;
        std::cout << regExprEngine1.IsMatched("abfffff") << std::endl;

        std::cout << regExprEngine2.IsMatched("ed") << std::endl;
        std::cout << regExprEngine2.IsMatched("eaaad") << std::endl;
        std::cout << regExprEngine2.IsMatched("ebbbbd") << std::endl;
        std::cout << regExprEngine2.IsMatched("ecd") << std::endl;

        std::cout << regExprEngine3.IsMatched("") << std::endl;
        std::cout << regExprEngine3.IsMatched("aa") << std::endl;
    }

    // context free test
    CfEngine cfEngine("/home/syple/code/expr_lexical_file.txt", "/home/syple/code/expr_file.txt", "/home/syple/code/lexical_file.txt");


    std::cout << "test over!" << std::endl;
    return 0;
}