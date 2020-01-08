#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cassert>
#include <set>
#include "./regular_expr/regexpr_engine.h"
#include "./matrix_io/matrix_io.h"

int Stoi(std::string str) {
    return std::stoi(str);
}

std::string ToString(int num) {
    return std::to_string(num);
}

int main() {
    // matrix io test
    std::vector<std::vector<int>> matrix(10, std::vector<int>(8));
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            matrix[i][j] = rand() % 100;
        }
    }
    MatrixIo<int> io("/home/syple/code/test.txt", Stoi, ToString);
    assert(0 == io.WriteFile(matrix));
    std::vector<std::vector<int>> rmatrix = io.ReadFile();
    assert(rmatrix.size() == 10);
    assert(rmatrix[0].size() == 8);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            assert(matrix[i][j] == rmatrix[i][j]);
        }
    }

    // regular expr test
    std::string repat = "b|c*";
    RegExprEngine regExprEngine(repat, true);
    if (regExprEngine.InitSuccess()) {
        std::cout << regExprEngine.IsMatched("b") << std::endl;
        std::cout << regExprEngine.IsMatched("c") << std::endl;
        std::cout << regExprEngine.IsMatched("cc") << std::endl;
    }

    std::cout << std::endl << "test over!" << std::endl;
    return 0;
}