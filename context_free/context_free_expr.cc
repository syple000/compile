
#include "./context_free_expr.h"

int ContextFreeExprUtil::IsNullable(ContextFreeSymbol* symbol, std::set<ContextFreeSymbol*>& dependentSymbols) {
    if (symbol->_nullable != 2) {
        return symbol->_nullable;
    }

    const auto& exprs = this->_exprMap.find(symbol->_key)->second;
    dependentSymbols.insert(symbol);

    int isNullable = 0;
    for (auto expr : exprs) {
        int nullable = ContextFreeExprUtil::IsExprNullable(expr, dependentSymbols);
        if (nullable == 1) {
            symbol->_nullable = nullable;
            isNullable = 1;
            break;
        }
        if (nullable == 2) {
            isNullable = 2;
        }
    }

    if (isNullable == 2) {
        // 存在循环判定，is nullable被赋值0
        isNullable = 0;
    }

    dependentSymbols.erase(symbol);
    symbol->_nullable = isNullable;
    return isNullable;
}

int ContextFreeExprUtil::IsExprNullable(ContextFreeExpr* expr, std::set<ContextFreeSymbol*>& dependentSymbols) {
    if (expr->_nullable != 2) {
        return expr->_nullable;
    }
    int nullable = 1;
    for (int i = 0; i < expr->_production.size(); i++) {
        int nodeNullable = expr->_production[i]->_nullable;
        if (nodeNullable == 2) {
            if (dependentSymbols.find(expr->_production[i]) == dependentSymbols.end()) {
                nodeNullable = ContextFreeExprUtil::IsNullable(expr->_production[i], dependentSymbols);
            }
        }

        if (nodeNullable == 1) {
            continue;
        } else if (nodeNullable == 2) {
            nullable = 2;
            continue;
        } else {
            nullable = 0;
            break;
        }
    }
    expr->_nullable = nullable;
    return nullable;
}