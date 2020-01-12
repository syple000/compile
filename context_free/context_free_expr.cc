
#include "./context_free_expr.h"

void ContextFreeExprUtil::GenNullable() {
    std::vector<ContextFreeSymbol*> symbolVec(this->_symbolMap.size(), nullptr);
    for (auto& it : this->_symbolMap) {
        symbolVec[it.second->_number] = it.second;
    }

    std::map<int, std::pair<std::vector<std::set<int>>, std::vector<std::set<int>>>> dependentInfo;
    for (auto symbol : symbolVec) {
        if (symbol->_state._nullable == 2) {
            int nullable = 0;
            std::vector<std::set<int>> dependingVec, dependedVec;
            auto& exprs = this->_exprMap.find(symbol->_key)->second;
            for (auto& expr : exprs) {
                std::set<int> dependingSet, dependedSet;
                int exprNullable = 1;
                for (auto& innerSymbol : expr->_production) {
                    if (innerSymbol->_state._nullable == 0) {
                        exprNullable = 0;
                        break;
                    } else {
                        if (innerSymbol->_state._nullable == 2) {
                            exprNullable = 2;
                        }
                        continue;
                    }
                }
                if ()
            }
        }
    }

}