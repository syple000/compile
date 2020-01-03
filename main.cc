#include "regular_expr/regular_expression_engine.h"
#include "iostream"

int main() {
    RegularExpressionEngine regularExprEngine = RegularExpressionEngine();
    regularExprEngine.IsMatched("aaaa");
    regularExprEngine.TransferStatus('a');
    std::cout << "test over!" << std::endl;
    return 0;
}