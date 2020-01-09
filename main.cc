#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cassert>
#include <set>
#include "./regular_expr/regexpr_engine.h"
#include "./io/io.h"

int Stoi(std::string str) {
    return std::stoi(str);
}

std::string ToString(int num) {
    return std::to_string(num);
}

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
    IO<int> io(Stoi, ToString);
    io.AppendVecToBuf(vec);
    io.AppendMatrixToBuf(matrix);
    assert(0 == io.WriteFile("/home/syple/code/test.txt", std::ios::out));
    io.SetIndexOfBuf(0);
    IO<int> newio(Stoi, ToString);
    newio.ReadFile("/home/syple/code/test.txt");
    std::vector<int> rvec = newio.GetVecFromBuf();
    std::vector<std::vector<int>> rmatrix = newio.GetMatrixFromBuf();
    assert(rvec.size() == 10);
    assert(rmatrix.size() == 10);
    assert(rmatrix[0].size() == 8);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            assert(matrix[i][j] == rmatrix[i][j]);
        }
        assert(rvec[i] == vec[i]);
    }

    // regular expr test
    // std::string repat = "((a)(b|cd*|e))f*";
    std::string repat = "((a)(b|cd*|e))f*";
    std::string repat2 = "e(a|bb|c)*d";
    RegExprEngine regExprEngine(repat, true);
    RegExprEngine regExprEngine2(repat2, true);
    if (regExprEngine.InitSuccess()) {
        std::cout << regExprEngine.IsMatched("ab") << std::endl;
        std::cout << regExprEngine.IsMatched("ae") << std::endl;
        std::cout << regExprEngine.IsMatched("acf") << std::endl;
        std::cout << regExprEngine.IsMatched("acdddff") << std::endl;
        std::cout << regExprEngine.IsMatched("abfffff") << std::endl;

        std::cout << regExprEngine2.IsMatched("ed") << std::endl;
        std::cout << regExprEngine2.IsMatched("eaaad") << std::endl;
        std::cout << regExprEngine2.IsMatched("ebbbbd") << std::endl;
        std::cout << regExprEngine2.IsMatched("ecd") << std::endl;
    }

    std::cout << std::endl << "test over!" << std::endl;
    return 0;
}