#include "./regular_expr/regexpr_engine.h"
#include "./matrix_io/matrix_io.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cassert>
#include <set>

int Stoi(std::string str) {
    return std::stoi(str);
}

std::string ToString(int num) {
    return std::to_string(num);
}

struct TestNode {
    int a;
    TestNode(): a(1) {}

    bool operator<(const TestNode& testNode) const {
        return a < testNode.a;
    }
};

class TestNodeCmp {
public:
    bool operator()(const TestNode* node1, const TestNode* node2) const {
        return node1->a < node2->a;
    }
};

int main() {
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
    TestNode* node = new TestNode(), *node1 = new TestNode();
    std::set<TestNode*> s;
    s.insert(node);
    s.insert(node1);

    std::set<TestNode*> sc;
    sc.insert(node);
    sc.insert(node1);

    std::cout << "test over!" << (s < sc ? "true" : "false") << (s > sc ? "true" : "false") << std::endl;
    return 0;
}