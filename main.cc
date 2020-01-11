#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cassert>
#include <set>
#include "./regular_expr/regexpr_engine.h"
#include "./io/io.h"
#include "./context_free/context_free_expr.h"

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
    io.AppendVecToBuf(vec);
    assert(0 == io.WriteFile("/home/syple/code/test.txt", std::ios::out));
    io.ReadFile("/home/syple/code/test.txt");
    std::vector<int> rvec = io.GetVecFromBuf();
    std::vector<std::vector<int>> rmatrix = io.GetMatrixFromBuf();
    assert(rvec.size() == 10);
    assert(rmatrix.size() == 10);
    assert(rmatrix[0].size() == 8);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            assert(matrix[i][j] == rmatrix[i][j]);
        }
        assert(rvec[i] == vec[i]);
    }
    assert(io.GetNextStrFromBuf() == "10");
    std::cout << io.GetNextLineFromBuf() << std::endl;
    std::cout << io.GetNextLineFromBuf() << std::endl;

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

    // 正则解析测试
    ContextFreeSymbol* symbol1 = new ContextFreeSymbol("A", "A", false, 2, 0);
    ContextFreeSymbol* symbol2 = new ContextFreeSymbol("B", "B", false, 2, 1);
    ContextFreeSymbol* symbol3 = new ContextFreeSymbol("a", "a", true, 0, 2);
    ContextFreeSymbol* symbol4 = new ContextFreeSymbol("b", "b", true, 0, 3);
    ContextFreeSymbol* symbol5 = new ContextFreeSymbol("c", "c", true, 1, 4);
    ContextFreeExpr* expr1 = new ContextFreeExpr();
    expr1->_nullable = 2;
    expr1->_sourceSymbol = symbol1;
    expr1->_production.push_back(symbol2);
    ContextFreeExpr* expr2 = new ContextFreeExpr();
    expr2->_nullable = 2;
    expr2->_sourceSymbol = symbol1;
    expr2->_production.push_back(symbol3);

    ContextFreeExpr* expr3 = new ContextFreeExpr();
    expr3->_nullable = 2;
    expr3->_sourceSymbol = symbol2;
    expr3->_production.push_back(symbol1);

    ContextFreeExpr* expr5 = new ContextFreeExpr();
    expr5->_nullable = 2;
    expr5->_sourceSymbol = symbol2;
    expr5->_production.push_back(symbol5);
    // ContextFreeExpr* expr4 = new ContextFreeExpr();
    // expr4->_nullable = 2;
    // expr4->_sourceSymbol = symbol2;
    // expr4->_production.push_back(symbol4);

    ContextFreeExprUtil util;
    util._exprMap["A"] = {expr1, expr2};
    util._exprMap["B"] = {expr3, expr5};
    std::set<ContextFreeSymbol*> dependentSymbols;
    util.IsNullable(symbol1, dependentSymbols);
    util.IsNullable(symbol2, dependentSymbols);
    std::cout << symbol1->_nullable << " " << symbol2->_nullable << std::endl;

    std::cout << "test over!" << std::endl;
    return 0;
}