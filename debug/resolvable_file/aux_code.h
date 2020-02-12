#define GEN_AUX_CODE_FILE 1
#ifndef AUX_CODE
#define AUX_CODE 1

#include "../../tree_node/tree_node.h"

#include "../../doc/symbol_table.h"

static SymbolTableManager *tableManager = new SymbolTableManager(10000);  

class AuxCode {
public:

    std::unordered_map<std::string, void(*)(CfTreeNode*, std::vector<CfTreeNode*>)> funcRegistry;

    void registFuncs() {
    }
};
#endif
