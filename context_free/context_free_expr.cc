
#include "./context_free_expr.h"

void ContextFreeExprUtil::GenNullable() {
    // 按_number元素排序符号
    std::vector<ContextFreeSymbol*> symbolVec(this->_symbolMap.size(), nullptr);
    for (auto& it : this->_symbolMap) {
        symbolVec[it.second->_number] = it.second;
    }
    // index = _number的非终结符号依赖与被依赖的关系; 同一set表示且关系, 相邻set表示或关系
    std::vector<std::vector<std::set<int>>> dependingVec;
    std::vector<std::set<int>> dependedVec;
    for (auto symbol : symbolVec) {
        if (symbol->_state._nullable == 2) {
            int nullable = 0;
            auto& exprs = this->_exprMap.find(symbol->_key)->second;
            std::vector<std::set<int>> vec;
            for (auto& expr : exprs) {
                int exprNullable = 1;
                std::set<int> set;
                for (auto& innerSymbol : expr->_production) {
                    if (innerSymbol->_state._nullable == 0) {
                        exprNullable = 0;
                        break;
                    } else {
                        if (innerSymbol->_state._nullable == 2) {
                            set.insert(innerSymbol->_number);
                            exprNullable = 2;
                        }
                        continue;
                    }
                }
                if (exprNullable == 1) {
                    symbol->_state._nullable = 1;
                    nullable = 1;
                    break;
                } else if (exprNullable == 2) {
                    nullable = 2;
                    vec.push_back(std::move(set));
                }
            }
            if (nullable == 1 || nullable == 0) {
                // 依赖于该符号的符号需要进行判定
            }
        }
    }

}