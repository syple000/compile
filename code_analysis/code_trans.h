
#ifndef CODE_GENERATION_CODE_TRANS
#define CODE_GENERATION_CODE_TRANS 1
/*
    代码检查规范： 生成语法树从根节点（由任一节点可以得到出该节点下的作用域等数据），逐层次往下分析作用域，数据检查
    代码生成规范： 某一节点（非叶节点）若可以由该节点以下的数据生成代码，则可立即生成;否则，需综合生成 
*/
#endif