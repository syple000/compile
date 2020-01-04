#include "string"

#ifndef _REGULAR_EXPR_ENGINE
#define _REGULAR_EXPR_ENGINE

namespace r_e {

    class RegularExpressionEngine {
    public:
        bool IsMatched(std::string);  // 当前正则引擎是否匹配该字符串
        int TransferStatus(char);  // 引擎在匹配当前字符后的状态
    };

}

#endif