#include <string>
#include <vector>

#include "../regular_expr/regexpr_engine.h"

#ifndef STRING_UTIL
#define STRING_UTIL 1

class StringUtil {
public:
    static std::vector<std::string> split(const std::string& str, const std::string& regExpr);
    static std::string trim(const std::string& str);
};

#endif